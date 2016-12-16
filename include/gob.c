/** \file gob.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "gob.h"

/**
 * Simply steps through the garbage chain, calling the cleanup function for each node,
 terminating when NULL.

 The cleanup function could quite simply be set to free().  But it should be set to something otherwise it will do nothing and skip the object, leaking it into memory.  In that case it really is your fault for not setting a cleanup function.

 If the cleanup function might be called independently of gob_cleanup -- ie a single object cleaned up at an arbitrary time, you must call gob_rm_garbage() from its cleanup function.
 */
void gob_cleanup( void * target ) {
    if( target == NULL ) return;


    gob * cur  = target;
    gob * next;

    while( cur != NULL ) {
	next = cur->nextgarb;
	if( cur->cleanup != NULL ) {
	    cur->cleanup( (void *)cur );
	}
	cur  = next;
    } 
}

/** If the cleanup function might be called independently of gob_cleanup -- ie a single object cleaned up at an arbitrary time, you must call gob_rm_garbage() from its cleanup function.
FIXME: I'm pretty sure this will bite me.  I deleted this call somewhere...
 */
int gob_rm_garbage ( void * dstp ) {
    if( dstp == NULL ) return 0;

    gob * dst = dstp;
    if( dst->prevgarb != NULL ) {
	gob * prevgarb = dst->prevgarb;
	prevgarb->nextgarb = dst->nextgarb;
    }
    if( dst->nextgarb != NULL ) {
	gob * nextgarb = dst->nextgarb;
	nextgarb->prevgarb = dst->prevgarb;
    }

    return 1;
}

int gob_set_garbage ( void * srcp, void * dstp ) {
    if( dstp == NULL ) return 0;

    gob * dst = dstp;
    gob * src = srcp;

    src->nextgarb = dst->nextgarb;
    src->prevgarb = dst;
    dst->nextgarb = src;
    if( src->nextgarb != NULL ) {
	gob * nextgarb = src->nextgarb;
	nextgarb->prevgarb = src;
    }

    return 1;
}

int gob_set_cleanup( void * dstp, void * cleanupfunc ) {
    if( dstp == NULL || cleanupfunc == NULL ) return -1;
    gob * dst = dstp;
    dst->cleanup = cleanupfunc;
}

void * gob_get_next( void * objp ) {
    if( objp == NULL ) return NULL;
    gob * obj = objp;

    return obj->next;
}

int gob_add_next( void * objp, void * nextp ) {
    if( objp == NULL ) return -1;
    gob * obj = objp;
    gob * next = nextp;

    next->next = obj->next;
    next->prev = obj;
    obj->next = next;

    return 1;
}
