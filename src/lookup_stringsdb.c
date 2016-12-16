#include <stdlib.h>
#include <string.h>
#include <kclangc.h>		/* Kyoto Cabinet */


struct hiena_lookup_strings_db {
    KCDB * db;
    Hserialno_t next_available_sn;	/* init this to 1; 0 is error code */
};

void * lookupstringsdb_init() {
    struct hiena_lookup_strings_db * lsdb = malloc(sizeof(*lsdb));
    if( lsdb == NULL ) {
	perror("hiena:lookupstringsdb_init: can't allocate db.\n");
	return NULL;
    }
    memset( lsdb, 0, sizeof(*lsdb) );

    lsdb->next_available_sn = 1;

    lsdb->db = kcdbnew();
    if( lsdb->db == NULL ) {
	perror("hienafs:lookupstringsdb_init: can't create new Kyoto Cabinet.\n");
	free(lsdb);
	return NULL;
    }
    if( kcdbopen(lsdb->db, "*", KCOWRITER) != 1 ) {	/* open as hash db */
	perror("hienafs:lookupstringsdb_init: can't open Kyoto Cabinet database.\n");
	kcdbdel(lsdb->db);
	return NULL;
    }


    return (void *)lsdb;
}

void lookupstringsdb_cleanup ( struct hiena_lookup_strings_db * lsdb ) {
    if( lsdb == NULL ) return;
    kcdbclose(lsdb->db);
    kcdbdel(lsdb->db);
    free(lsdb);
}

Hserialno_t lookupstringsdb_get_serialno ( struct hiena_lookup_strings_db * lsdb, const char * string, size_t len ) {
    if( lsdb == NULL ) return 0;
    if( lsdb->db == NULL ) return 0;
    struct return_wrapper {
	Hserialno_t sn;
    };
    char * rbuf;
    size_t rlen;
    rbuf = kcdbget( lsdb->db, string, len, &rlen );
    if( rbuf == NULL ) return 0;
    struct return_wrapper * wrapper = (struct return_wrapper *)rbuf;

    Hserialno_t sn = wrapper->sn;
    kcfree(wrapper);

    return sn;
}

Hserialno_t lookupstringsdb_put_string ( struct hiena_lookup_strings_db * lsdb, const char * string, size_t len ) {
    if( lsdb == NULL ) return 0;
    if( lsdb->db == NULL ) return 0;

    /* CHECK FOR EXISTING STRING */

    Hserialno_t sn = lookupstringsdb_get_serialno( lsdb, string, len );
    if( sn != 0 ) return sn;

    /* CREATE NEW SERIAL NUMBER */
    sn = lsdb->next_available_sn++;

    /* ADD SERIAL NUMBER TO DATABASE, USING STRING AS KEY */
    if( kcdbadd( lsdb->db, string, len, (const char *)&sn, sizeof(sn) ) == 0 ) {
	perror("hiena:lookupstringsdb_put_string: failed.\n");
	return 0;
    }
    return sn;
}

