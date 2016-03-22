/** \file gob.c
 */

#include <stdlib.h>
#include "gob.h"

/**
 * Simply steps through the garbage chain, calling the cleanup function for each node,
 terminating when NULL.

 The cleanup function could quite simply be set to free().  But it should be set to something otherwise it will do nothing and skip the object, leaking it into memory.  In that case it really is your fault for not setting a cleanup function.
 */
void gob_cleanup( void * target ) {
    if( target == NULL ) return;
    gob * cur  = target;
    gob * next;

    while( cur != NULL ) {
	next = cur->nextgarb;
	if( cur->cleanup != NULL )
	    cur->cleanup( (void *)cur );
	cur  = next;
    } 
}
void gob_add_nextgarb( void * dstp, void * srcp ) {
    if( dstp == NULL ) return;
    gob * dst = dstp;
    gob * src = srcp;
    dst->nextgarb = src;
    src->prevgarb = dst;
}

void gob_set_cleanup( void * dstp, void * srcp ) {
    if( dstp == NULL ) return;
    gob * dst = dstp;
    dst->cleanup = srcp;
}

