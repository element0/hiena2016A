/** @file scanner_callbacks.c */

#include "include/hiena/scanner_module.h"


void * scanner_new_mapcel ( off_t pos, size_t len, Htok relative_element_id, Hsp * h ) {
    return NULL;
}

int    scanner_set_val ( void * dstcel, void * ptr, size_t len );


Hscb* scanner_callbacks_init () {
    Hscb * cb = malloc(sizeof(*cb));
    if( cb == NULL ) return NULL;

    cb->new_mapcel = scanner_new_mapcel;
    cb->add_mapcel = scanner_add_mapcel;
    cb->add_child  = scanner_add_child;
    cb->set_val    = scanner_set_val;

    return cb;
}


/* end file scanner_callbacks.c */
