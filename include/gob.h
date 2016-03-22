/* _RAYGAN_GOB_H_ */
#ifndef _RAYGAN_GOB_H_
#define _RAYGAN_GOB_H_

/** \file gob.h
 *
 */

typedef struct raygan_general_object gob;

/**
 * Subclass this object by making it the first data field in a structure
 * or by making a subclass of this object the first data field in a structure.
 *
 * This object provides for a doubly linked list
 * and for a doubly linked garbage collection chain.
 * The two are not the same...
 *
 * ACCESSOR METHODS:
 * None.
 * Keep it simple and just dereference after typecasting your own
 * subclassed object to a gob.
 */
struct raygan_general_object
{
    void * prev;	/**< linked list stub */
    void * next;	/**< linked list stub */
    void * prevgarb;	/**< garbage collector stub */
    void * nextgarb;	/**< garbage collector stub */
    void (*cleanup)( void * );	/**< cleanup function for custom object */
};

/** GARBAGE COLLECTION METHOD.
 * this method name could change.  but I want to keep it simple.
 */
void cleanup_garbage( void * );
void gob_add_nextgarb( void * dstp, void * srcp );
void gob_set_cleanup( void * dstp, void * srcp );


#endif /*!_RAYGAN_GOB_H_*/
