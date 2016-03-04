#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>	/* uint64_t */

#include "hpat.h"

static const int sizeof_hpat = sizeof(Hpat);	/* i suppose this will optimise things? */


Hpat *new_hpat()
{
    Hpat *hpat = malloc(sizeof_hpat);
    if(hpat != NULL)
	memset(hpat,0,sizeof_hpat);
    return hpat;
}
Hpat *new_hpat_from_buf(off_t len, void *buf) {
    if(len==0 || buf==NULL)
	return NULL;
    Hpat *hpat = new_hpat();
    if(hpat != NULL) {
	hpat->buf  = buf;
	hpat->len  = len;
    }
    return hpat;
}
Hpat *new_hpat_from_string(const char *str) {
    if(str==NULL)
	return NULL;
    Hpat *hpat = new_hpat();
    if(hpat != NULL) {
	hpat->len  = strlen(str);
	hpat->buf  = strndup(str, hpat->len);
    }
    return hpat;
}
Hpat *new_hpat_from_str(const char *str) {
    return new_hpat_from_string(str);
}
Hpat *hpat_from_str(const char *str) {
    return new_hpat_from_string(str);
}

Hpat *new_hpat_from_uint64(uint64_t num) {
    Hpat *hpat = new_hpat();
    if(hpat == NULL)
	return NULL;
    hpat->buf  = malloc(hpat->len);
    if(hpat->buf != NULL) {
	hpat->len  = sizeof(uint64_t);
	memcpy(hpat->buf,&num,hpat->len);
    }

    return hpat;
}
Hpat *hpat_from_uint64(uint64_t num) {
    return new_hpat_from_uint64(num);
}

Hpat *new_hpat_from_argv(int argc, char *argv[])
{
    size_t size = 0;
    int i = 0;

    if (argc>0){

	while(i<argc){
	    size = size + sizeof(char)*( strlen(argv[i])+1 );
	    i++;
	}

	void *buf = malloc(size);
	if(buf == NULL)
	    return NULL;

	void *pos = buf;
	size_t strsize = 0;
	for(i=0;i<argc;i++){
	    strsize = strlen(argv[i])+1;
	    bcopy((void *)argv[i], pos, strsize);
	    pos = pos+strsize;
	}

	Hpat *bufob = malloc(sizeof_hpat);
	if(bufob == NULL) {
	    free(buf);
	    return NULL;
	}
	bufob->buf = buf;
	bufob->len = size;
	return bufob;

    }else{
	/* if argc is 0 or less, we have no data */
	/* TBD ERR */
	return NULL;
    }
}
Hpat *hpat_from_argv(int argc, char *argv[]) {
    return new_hpat_from_argv(argc, argv);
}


char *hpat_str_ptr(Hpat *pat) {
    /* function fails if not NUL terminated */
    char *s=NULL;

    if(pat!=NULL)
	if(pat->buf!=NULL){
	    s = (char *)pat->buf;
	    if(s[pat->len]=='\0')
		return s;
	}
    return NULL;
}

Hpat *dupe_hpat(Hpat *pat1) {
    if(pat1==NULL)
	return NULL;

    Hpat *pat2 = new_hpat();
    if(pat2 == NULL)
	return NULL;

    pat2->buf = malloc(pat2->len);
    if(pat2->buf != NULL) {
	pat2->len = pat1->len;
	memcpy(pat2->buf, pat1->buf, pat2->len);
    }

    return pat2;
}
Hpat *hpat_dupe(Hpat *pat) {
    return dupe_hpat(pat);
}


int hpat_is_c_string(Hpat *p) {	/* := TRUE | FALSE */
    if(p == NULL)
    {
	return 0;
    }
    char *s = (char *)p->buf;
    if(s[p->len]=='\0')
    {
	return 1;
    }
    return 0;
}


int compare_hpat(Hpat *pat1, Hpat *pat2) {
    if(pat1 != NULL && pat2 != NULL) {
	if(pat1->buf != NULL && pat2->buf != NULL) {
	    if(pat1->len == pat2->len) {
		return(memcmp(pat1->buf, pat2->buf, pat1->len));
	    }
	}
    }
    return -1;  // failure
}
int hpat_cmp(Hpat *pat1, Hpat *pat2) {
    return compare_hpat(pat1, pat2);
}


FILE *hpat_fopen(Hpat *pat, char *mode) {
    if(pat != NULL && mode != NULL) {
	return fmemopen(pat->buf, pat->len, mode);
    }
    return NULL;
}

int cleanup_hpat(Hpat *hpat) {
    if (hpat != NULL) {
	if(hpat->buf != NULL) {
	    memset(hpat->buf, 0, hpat->len);	// SECURITY: call me paranoid.
	    free(hpat->buf);
	}
        free(hpat);
    }
    return 0;
}
int hpat_cleanup(Hpat *hpat) {
    return cleanup_hpat(hpat);
}
