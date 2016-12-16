#include <hiena/lookup_module.h>



/**
  Init lookup frame:
  - set callbacks
 */
struct hiena_lookup_frame * lookup_frame_init ( Hcb *cb,
	void * addr, size_t addrlen, struct hiena_serverops * server,
	struct hiena_server_callbacks * servcb ) {

    struct hiena_lookup_frame * h = malloc( sizeof( *h ) );
    memset( h, 0, sizeof( *h ) );

    Hcb *dupcb = malloc(sizeof( *cb ));
    h->op = memcpy( dupcb, cb, sizeof( *cb ) );	/* this way we can modify
						   the callbacks if we need */
    h->addr = addr;
    h->addrlen = addrlen;
    h->server = (void *)server;
    h->servcb = (void *)servcb; 

    return h;
}

void lookup_frame_cleanup ( struct hiena_lookup_frame * h ) {
    if( h == NULL ) return;
    if( h->op != NULL ) free( h->op );
    free(h);
}
