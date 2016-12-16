#include <stdlib.h>
#include <string.h>
#include <gob.h>

#include "domaincell.h"
#include "domaincell_callbacks.c"



void domaincell_cleanup ( struct hiena_domaincell * dc ) {
    if( dc == NULL ) { 
	return;
    }
    if( (dc->addr != NULL) && (dc->addr_cleanup != NULL) ) {
	dc->addr_cleanup( dc->addr );
    }
    if( (dc->serversign != NULL ) && (dc->serversign_cleanup != NULL ) ) {
	dc->serversign_cleanup( dc->serversign );
    }
    free( dc );
}

struct hiena_domaincell * domaincell_new ( void * garbage ) {
    struct hiena_domaincell * dc = malloc( sizeof( *dc ));
    memset( dc, 0, sizeof( *dc ) );
    gob_set_garbage( dc, garbage );
    gob_set_cleanup( dc, domaincell_cleanup );
    dc->op = hiena_domaincell_callbacks;
    return dc;
}


int domaincell_set_address ( dcel * dc, void * addr, size_t addr_len, void * cleanupfunc ) {
    if( dc == NULL || cleanupfunc == NULL ) return -1;
    dc->addr.protocol_addr = addr;
    dc->addr_cleanup = cleanupfunc;
    return 1;
}

int domaincell_set_server ( dcel * dc, void * namestr, void * cleanupfunc ) {
    if( dc == NULL ) return -1;
    dc->serversign = namestr;
    dc->serversign_cleanup = cleanupfunc;
    return 1;
}

int domaincell_set_serverlib ( dcel * dc, void * serverlib ) {
    if( dc == NULL ) return -1;
    dc->addr.serverlib = serverlib;
    return 1;
}

void * domaincell_get_serverlib ( dcel * dc ) {
    if( dc == NULL ) return NULL;
    return dc->serverlib;
}


