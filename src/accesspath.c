#include <gob.h>
#include "accesspath.h"

Axpa * accesspath_new ( ) {
    Axpa *a = malloc(sizeof(Axpa));
    if(a == NULL) return NULL;
    memset(a, 0, sizeof(Axpa));

    return a;
}

void accesspath_cleanup ( Axpa *a ) {
    if( a == NULL ) return;
    // gob_rm_garbage( a );
    /* hiena_axpa_destroy( a->child ); */
    /* hiena_axpa_destroy( a->next ); */
    /* hiena_scas_destroy( a->scas ); */
    /* cleanup_hpat( a->name ); */
    free(a);
}

int accesspath_set_garbage( Axpa *a, Axpa *garbage ) { /* 1 | 0 */  /* CHANGED 3-3-16 return value */
    if( a == NULL ) return 0;

    if( garbage != NULL ) {
	gob_set_garbage( garbage, a );
    }
    gob_set_cleanup( a, accesspath_cleanup );

    return 1;
}



void accesspath_refresh ( Axpa *a ) {
    if(a == NULL) return;

    /* update slib cascade:
       check sync between parent Axpa and current axpa's slibs. */

    /* update parent domain:
       check sync between parent domain reference
       and the parent access path. */

    /* update selection domain stream:   
       check sync between domain stream and domain server
           flush if needed, reload if needed */

    /*** VERY ROUGH ***/
    //Dpak *domain = a->domain;

    //do_scanners(domain, a->scanners);




    /* update map and lists:
       check sync between slib and domain maps and lists */
    /* update request expression:
       check sync between domain and expr
           refresh domain
	   -or-
	   refresh request expression ("rql")	*/
}


/*---- SETTERS ----*/



/* TBD: RENAME: axpa_load_scanlib */
/*
int axpa_set_scanlib(Axpa *a, const char *scanlibpaths) {
    if(a == NULL || scanlibpaths == NULL) {
	fprintf(stderr, "axpa_set_scanlib: Axpa or scanlibpath NULL, abort.\n");
	fflush(stderr);
	return -1;
    }
    a->scanners = cosmosfs_slib_scanners_init(scanlibpaths);
    if(a->scanners == NULL) {
	fprintf(stderr,"axpa_set_scanlib: can't create scanners, abort.\n");
	fflush(stderr);
	return -1;
    }
    return 1;
}
*/

/*---- OUTPUTTERS ----*/

/* "print_dir" and "print_stream"
   both imply the "selection" packet of the Axpa */

/* extremely dirty implementation */
/*
void axpa_print_dir ( Axpa *a ) {
    if(a == NULL) return;
    if(a->domain == NULL) return;
    if(a->domain->first_child == NULL) return;

    Dpak *p = a->domain;
    list_t *cur_row = NULL;
    Dpak *cur_rowpak = NULL;
    list_t *first_col = NULL;
    Dpak *first_colpak = NULL;
    char *colstr = NULL;
    while((cur_row = get_next_row(p,cur_row)) != NULL) {
	cur_rowpak = list_get_ob(cur_row);
	first_col  = ppak_get_next_col(cur_rowpak, NULL);
	first_colpak = list_get_ob(first_col);
	*/
	//colstr     = ppak_get_str(first_colpak);	/* colstr must be free'd */
/*
	printf("%s\n", colstr);
	free(colstr);
    }
}
*/
/*
void axpa_print_stream ( Axpa *a ) {
    if(a == NULL) return;
    if(a->domain == NULL) return;
    if(a->domain->val.buf == NULL) return;

    Hpat *hp = a->domain->val.buf;

    void *buf = hp->buf;
    char *c;
    for(; buf < buf+hp->len; buf++) {
	c = (char *)buf;
	putchar(*c);
    }

}
*/



/*--------*/
