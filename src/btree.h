/* RAYGAN_BTREE_H */

#ifndef _RAYGAN_BTREE_H_
#define _RAYGAN_BTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define RBT_NUM_KEYS  5
#define RBT_EMPTY 0
#define RBT_ROOTKEY 1
#define RBT_MAX_INSERTS 20

typedef ino_t bkey_t;

typedef struct bnode bnode_t;
typedef struct bnode
{
    bkey_t   key [RBT_NUM_KEYS];
    bnode_t *link[RBT_NUM_KEYS+1];	/* cleanup by btree_cleanup() */
    void    *data[RBT_NUM_KEYS];	/* make sure to implement your own garbage
				   collector for this object */
    bnode_t *garbage;
}bnode_t;

typedef struct raygan_btree btree_t;
typedef struct raygan_btree
{
    bnode_t *root;
    bnode_t *garb;	/* garbage */
}btree_t;


btree_t * btree_create();
int       btree_put( btree_t *, bkey_t *, bval_t *, size_t );
void    * btree_get( btree_t *, bkey_t * ); 
void      btree_cleanup( btree_t * );

//bnode_t * btree_add( bnode_t *, bkey_t );
//void      btree_cleanup( bnode_t * );



#endif /*!_RAYGAN_BTREE_H_*/
