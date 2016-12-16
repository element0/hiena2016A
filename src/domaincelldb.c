#include <stdlib.h>
#include <string.h>
#include <fuse_lowlevel.h>	/* fuse_ino_t */
#include "domaincell.c"

struct hiena_domaincell_db {
    struct hiena_domaincell * dsroot;
};

typedef fuse_ino_t ds_ino_t;

void domaincelldb_cleanup ( struct hiena_domaincell_db * dsdb ) {
    if( dsdb == NULL ) return;
    gob_cleanup( dsdb->dsroot );
    free( dsdb );
}

struct hiena_domaincell_db * domaincelldb_init ( ) {
    struct hiena_domaincell_db * dsdb = malloc(sizeof(*dsdb));
    if( dsdb == NULL ) {
	perror("hiena:domaincelldb_init: failed to allocate struct hiena_domaincell_db.\n");
	return NULL;
    }
    memset( dsdb, 0, sizeof(*dsdb) );
    dsdb->dsroot = domaincell_new( NULL );
    if( dsdb->dsroot == NULL ) {
	perror("hiena:domaincelldb_init: failed to allocate root domaincell.\n");
	domaincelldb_cleanup( dsdb );
	return NULL;
    }
    
    return dsdb;
}

Hds * domaincelldb_get_domaincell ( struct hiena_domaincell_db * dsdb, ds_ino_t dsno ) {
    if( dsdb == NULL ) return NULL;

    /* TODO: Temporary */
    if( dsno == 1 ) {
	return dsdb->dsroot;
    }

    return NULL;
}
