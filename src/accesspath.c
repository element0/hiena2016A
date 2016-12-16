#include <gob.h>
#include <sys/stat.h>
#include <string.h>

#include "accesspath.h"
#include "accesspath_branches.c"

Axpa * accesspath_new ( ) {
    Axpa *a = malloc(sizeof(Axpa));
    if(a == NULL) return NULL;
    memset(a, 0, sizeof(Axpa));

    return a;
}

void accesspath_cleanup ( Axpa *a ) {
    if( a == NULL ) return;
    if( a->lookup_cache != NULL ) {
	accesspath_branches_cleanup( a->lookup_cache );
    }
    /* hiena_axpa_destroy( a->child ); */
    /* hiena_axpa_destroy( a->next ); */
    /* hiena_scas_destroy( a->scas ); */
    /* cleanup_hpat( a->name ); */
    free(a);
}


/** garbage methods 

 */

int accesspath_set_garbage( Axpa * a, Axpa * garbage ) { /* 1 | 0 */  /* CHANGED 3-3-16 return value */
    if( a == NULL ) return 0;

    if( garbage != NULL ) {
	gob_set_garbage( a, garbage );
    }
    gob_set_cleanup( a, accesspath_cleanup );

    return 1;
}

/** sync flags methods 

 */

int accesspath_set_syncflags ( Axpa * a, int sf ) {
    if( a == NULL ) return 0;
    a->syncflags = sf;
    return 1;
}

int accesspath_get_syncflags ( Axpa * a ) {
    if( a == NULL ) return -1;
    return a->syncflags;
}


/** parent methods 
 
 */

static fuse_ino_t accesspath_get_parent_ino ( Axpa * ap ) {
    if( ap == NULL || ap->parent == NULL ) return 0;
    return ap->parent->stbuf.st_ino;
}

Axpa * accesspath_get_parent ( Axpa * ap ) {
    if( ap == NULL ) return NULL;
    return ap->parent;
}

int accesspath_set_parent ( Axpa * ap, Axpa * parent ) {
    if( ap == NULL ) return 0;
    ap->parent = parent;
    return 1;
}


/** cascading configuration methods

 */

int accesspath_set_cascadingconfigdirname( Axpa * ap, char * name, size_t len ) {
    if( ap == NULL ) return 0;
    if( name == NULL || len == 0 ) {
	ap->cascadingconfigdirname[0] = '\0';
	return 1;
    }
    size_t copysize;
    if( len <= HIENA_CASCADING_CONFIG_DIRNAME_ALLOCATION -1 ) {
	copysize = len;
    }else{
	copysize = HIENA_CASCADING_CONFIG_DIRNAME_ALLOCATION -1;
    }
    memcpy( ap->cascadingconfigdirname, name, copysize );
    ap->cascadingconfigdirname[copysize] = '\0';
    ap->cascadingconfigdirnamesize = copysize;


    printf("ap->cascadingconfigdirname = %s\n", ap->cascadingconfigdirname );

    return 1;
}



/** scanner library methods
 
 */

int accesspath_set_scannerlib ( Axpa * ap, void * scannerlib ) {
    if( ap == NULL ) return 0;
    ap->scannerlib = scannerlib;
    return 1;
}


/** do not use this scannerlib to process the domain of the access path.
 
 */
struct hiena_scannerlib * accesspath_get_scannerlib ( Axpa * ap ) {
    if( ap == NULL ) return NULL;
    return ap->scannerlib;
}


/** use this scannerlib to process the access path's domain.

 */
struct hiena_scannerlib * accesspath_get_parent_scannerlib ( Axpa * ap ) {
    if( ap == NULL || ap->parent == NULL ) return NULL;
    /** this is important because *technically* the scannerlib
        is a product of the domain of the access path.
     */
    return ap->parent->scannerlib;
}


/** lookup cache methods 

 */

Hino_t accesspath_get_branch_ino( Axpa * ap, Hserialno_t sn ) {
    if( ap == NULL || sn == 0 ) return 0;
    if( ap->lookup_cache == NULL ) {
	ap->lookup_cache = accesspath_branches_init();
	return 0;
    }

    struct hiena_accesspath_branch * cur = ap->lookup_cache;

    while( cur != NULL ) {
	if( cur->key == sn )
	    return cur->val;
	cur = gob_get_next(cur);
    }
    return 1;
}

/** this returns -1 on error, 1 on success.  places a pointer to its own ivar
  'cascadingconfigdirname' in 'nameptr' and the size in 'namesize'
 */
int accesspath_get_cascadingconfigdirname_ptr( Axpa * ap, char **nameptr, size_t *namesize ) {
    if( ap == NULL || nameptr == NULL || namesize == NULL ) return 0;
    printf("accesspath_get_cascadingconfigdirname_ptr: %s\n", ap->cascadingconfigdirname );
    *nameptr  = ap->cascadingconfigdirname;
    *namesize = ap->cascadingconfigdirnamesize;
    return 1;
}



/** stat buffer methods

 */

static int accesspath_set_stbuf ( Axpa * ap, struct stat * stbuf ) {
    if( ap == NULL || stbuf == NULL ) return 0;
    memcpy( &ap->stbuf, stbuf, sizeof(ap->stbuf) );
    return 1;
}

static int accesspath_get_stbuf ( Axpa * ap, struct stat * stbuf ) {
    if( ap == NULL || stbuf == NULL ) return 0;
    memcpy( stbuf, &ap->stbuf, sizeof(*stbuf) );
    return 1;
}


/** domain methods

 */
int accesspath_set_domain ( Axpa * ap, Hds * ds ) {
    if( ap == NULL ) return 0;
    ap->domain = ds;
    return 1;
}

Hds * accesspath_get_domain ( Axpa * ap ) {
    if( ap == NULL ) return NULL;
    return ap->domain;
}


/** access path methods

 */
int accesspath_refresh ( Axpa *a ) {
    if(a == NULL) return 0;

    /** the access path's domain may contain a cascading configuration.
        such configuration cannot be accessed until after the domain is parsed.
	such configuration will affect the parsing of children,
	but not the parsing of the domain.  therefor, the domain must be parsed
	by the parent access path's configuration.

	we syncronize the accesspath's parent first.
     */
    Axpa * par = accesspath_get_parent( a );

    /** not in sync */
    if( par->syncflags == 0 ) {
	/** recursive refresh */
	accesspath_refresh( par );

	/** reload scanners.
	    quick n dirty version just copies the parent's scannerlib.
	    TODO: THIS SHOULD BE REPLACED BY CASCADING SYSTEM
	 */
	par->scannerlib = accesspath_get_parent_scannerlib( par );
    }

    /** set sync flags */

    accesspath_set_syncflags( a, 1 );

    return 1;
}

