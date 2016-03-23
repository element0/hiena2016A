#include "accesspathdb.h"
#include "string.h"

/* DIRECT CODE INCLUDE */
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
axno_t accesspathdb_add_accesspath ( Axpadb *ax, Axpa *axpa ) { /* >0 | 0 */
    if(ax == NULL || ax->ind == NULL || axpa == NULL) return 0;

    KCDB * db = ax->ind;
    
    axno_t axno  = accesspathdb_new_axno( ax );
    if( axno == 0 ) {
	fprintf(stderr, "accesspathdb_add_accesspath: accesspathdb_new_axno returned 0. returning 0.\n");
	return 0;
    }
   
    /* WORKING NOTE: the key should be a axno_t and the value should be the value of a pointer */
    if( kcdbadd(db, (const char *)&axno, sizeof(axno_t), (const char *)&axpa, sizeof(axpa)) != 1 ) {
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

    return axno;
}

axno_t accesspathdb_new_accesspath ( Axpadb * ax ) {
    if( ax == NULL ) return 0;

    Axpa * ap = accesspath_new();
    if( ap == NULL ) return 0;

    axno_t an = accesspathdb_add_accesspath( ax, ap );
    if( an == 0 ) { 
	accesspath_cleanup( ap );
	return 0;
    }

    return an;
}


/**
 * Initialize a fresh access path database.  Include a root access node.
 */
Axpadb * accesspathdb_init () {
    Axpadb * ax = malloc(sizeof(*ax));
    memset(ax, 0, sizeof(*ax));

    ax->next_axno = 1;		/* MUST initialize to 1, 0 is error code */
    KCDB * db = ax->ind = kcdbnew();
    
    if ( kcdbopen(db, "+", KCOWRITER) != 1 )	/* open as B+ tree */
	goto abort;

    axno_t rootno = accesspathdb_new_accesspath( ax );
    if( rootno == 0 ) {
	/* root access path node number should return 1
	   otherwise, always check for a return status of 0 for ERROR */
	goto abort;
    }
    printf("TESTED: accesspathdb_new_accesspath() returned %d\n", rootno);
    axno_t testno = accesspathdb_new_accesspath( ax );
    if( testno != 0 ) {
	printf("TESTED: accesspathdb_new_accesspath() returned %d\n", testno );
    }
    axno_t testn2 = accesspathdb_new_accesspath( ax );
    if( testn2 != 0 ) {
	printf("TESTED: accesspathdb_new_accesspath() returned %d\n", testn2 );
    }
    return ax;
abort:
    kcdbdel(db);
    return NULL;
}

void accesspathdb_cleanup ( Axpadb * ax ) {
    if(ax == NULL) return;
    if(ax->axroot != NULL) {
	gob_cleanup( ax->axroot );
    }
    if(ax->ind != NULL) {
	KCDB * db = ax->ind;
	kcdbclose(db);
	kcdbdel(db);
    }
    free(ax);
}
