#include <string.h>

#include "accesspathdb.h"
#include "accesspath.c"


/**
 * Just get the next access path index number.
 *
 */
static axno_t accesspathdb_new_axno ( Axpadb *ax ) {  /* 0 == ERROR */
    if(ax == NULL) return 0;
    /* overflow logic -- make sure axno_t is UNSIGNED or rewrite this overflow logic */
    if(ax->next_axno == (axno_t)-1) {
	fprintf(stderr, "accesspathdb_new_axno: axno_t overflow detected. returning 0.\n");
	return 0;
    }
    return ax->next_axno++;
}


/**
 * Add an access path to the access path index, return its new access number.
 *
 */
axno_t accesspathdb_add_accesspath ( Axpadb *ax, Axpa *axpa, Axpa * par ) { /* >0 | 0 */
    if(ax == NULL || ax->db == NULL || axpa == NULL) return 0;

    KCDB * db = ax->db;
    
    axno_t axno  = accesspathdb_new_axno( ax );
    if( axno == 0 ) {
	fprintf(stderr, "accesspathdb_add_accesspath: accesspathdb_new_axno returned 0. returning 0.\n");
	return 0;
    }
  
    if( kcdbadd(db, (const char *)&axno, sizeof(axno_t), (const char *)&axpa, sizeof(Axpa *)) != 1 ) {
	fprintf( stderr, "accesspathdb_add_accesspath: kcdbadd failed.  (%s)  returning.\n", kcdbemsg(db));
	return 0;
    }

    if( ax->axroot == NULL && axpa != NULL ) {
	ax->axroot = axpa;
	accesspath_set_garbage( axpa, NULL );

    }else if( accesspath_set_garbage( axpa, ax->axroot ) != 1 ) {
	fprintf( stderr, "accesspathdb_add_accesspath: axpa_set_garbage failed. returning.\n");
	return 0;
    }

    axpa->parent = par;

    return axno;
}


Axpa * accesspathdb_get_accesspath ( Axpadb * ax, axno_t axno ) {
    struct bone_head_wrapper {
	Axpa *ap;
    };
    size_t ptrsize;

    if( ax == NULL ) return NULL;
    char * ptr = kcdbget( ax->db, (const char *)&axno, sizeof(axno_t), &ptrsize );
    if( ptr == NULL ) return NULL;
    struct bone_head_wrapper * value = (struct bone_head_wrapper *)ptr;

    void * retp = value->ap;

    kcfree(ptr);

    return retp;
}

Axpa * accesspathdb_get_axproto ( Axpadb * ax ) {
    if( ax == NULL ) return NULL;
    return ax->axproto;
}

axno_t accesspathdb_new_accesspath ( Axpadb * ax, axno_t parno ) {
    if( ax == NULL ) return 0;

    Axpa * ap = accesspath_new();
    if( ap == NULL ) return 0;

    Axpa * parap = NULL;
    if( parno > 0 )
	parap = accesspathdb_get_accesspath( ax, parno );

    axno_t an = accesspathdb_add_accesspath( ax, ap, parap );
    if( an == 0 ) { 
	accesspath_cleanup( ap );
	return 0;
    }

    char * nameptr = NULL;
    size_t namesize = 0;
    accesspath_get_cascadingconfigdirname_ptr( parap, &nameptr, &namesize );
    accesspath_set_cascadingconfigdirname( ap, nameptr, namesize );


    return an;
}


/**
 * Initialize a fresh access path database.  Include a root access node.
 */
Axpadb * accesspathdb_init () {

    Axpadb * ax = malloc(sizeof(*ax));
    memset(ax, 0, sizeof(*ax));

    ax->next_axno = 1;		/* MUST initialize to 1, 0 is error code */
    KCDB * db = ax->db = kcdbnew();
    
    if ( kcdbopen(db, "+", KCOWRITER) != 1 )	/* open as B+ tree */
	goto abort;

    /********************/
    /* ROOT ACCESS PATH */
    /********************/

    axno_t rootno = accesspathdb_new_accesspath( ax, 0 );
    if( rootno == 0 ) {
	/* root access path node number should return 1
	   otherwise, always check for a return status of 0 for ERROR */
	goto abort;
    }
    Axpa * axroot = accesspathdb_get_accesspath( ax, rootno );

    /** an accesspath's stbuf will normally be set by the lookup subsystem
     *  we need to bootstrap this for root access path.
     */
    struct stat stbuf;
    stbuf.st_ino = 1;
    stbuf.st_mode = S_IFDIR | 0755;
    stbuf.st_nlink = 1;			/**< following Linux's cue, simply use 1 for st_nlink */
    accesspath_set_stbuf( axroot, &stbuf );

    printf("TESTED: accesspathdb_new_accesspath() returned %d\n", rootno);


    /*********************/
    /* PROTO ACCESS PATH */
    /*********************/

    /** The proto access path contains the host installed cascading configuration.
        The proto access path is necessary to support scanning of the root domain
	and also necessary for the root access path to have a cascading configuration
	to inherit from.

	It must be created _after_ root in order not to have ino 1 (root).
     */
    
    axno_t protono = accesspathdb_new_accesspath( ax, 0 );
    if( protono == 0 ) {
	perror("accesspathdb: failed to init proto access path.\n");
	goto abort;
    }
    Axpa * axproto = accesspathdb_get_accesspath( ax, protono );
    ax->axproto = axproto;

    /* set cascading configuration directory name for proto */
    char * nameptr = HIENA_CASCADING_CONFIG_DIRNAME;
    int    namelen = strlen(nameptr);
    accesspath_set_cascadingconfigdirname( axproto, nameptr, namelen );

    /* set cascading configuration directory name for root.
       this is another bootstrap.  usually the configuration name would be 
       inherited during accesspathdb_new_accesspath() but
       because proto wasn't around, here you go. */
    accesspath_set_cascadingconfigdirname( axroot, nameptr, namelen );

    /* set proto as root's parent */
    accesspath_set_parent( axroot, axproto );

    /*************************/
    /* RETURN ACCESS PATH DB */
    /*************************/

    return ax;
abort:
    kcdbdel(db);
    return NULL;
}


/**
 * CLEANUP
 */

void accesspathdb_cleanup ( Axpadb * ax ) {
    if(ax == NULL) return;
    if(ax->axroot != NULL) {
	gob_cleanup( ax->axroot );
    }
    if(ax->db != NULL) {
	KCDB * db = ax->db;
	kcdbclose(db);
	kcdbdel(db);
    }
    free(ax);
}
