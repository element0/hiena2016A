/* RAYGAN_BTREE_C */

#include "btree.h"

bnode_t *new_bnode (bnode_t *garbage)
{
    bnode_t *bnode = malloc(sizeof(*bnode));
    memset(bnode, RBT_EMPTY, sizeof(*bnode));

    if(garbage != NULL) {
	bnode->garbage = garbage->garbage;
	garbage->garbage = bnode;
    }
    
    return bnode;
}

void bnode_cleanup (bnode_t *node) {
    free(node);
}

void bnode_init_root (bnode_t *root_n) {
    if(root_n != NULL) {
	root_n->key[0] = RBT_ROOTKEY;
    }
}

btree_t * btree_create ( ) {
    btree_t *t = malloc(sizeof(*t));
    memset(t, 0, sizeof(*t));

    t->root = new_bnode(NULL);
    t->garb = t->root;

    return t;
}

void btree_cleanup ( btree_t *t ) {
    bnode_t *cur_n = t->garb;
    bnode_t *next_n;
    while(cur_n != NULL) {
	next_n = cur_n->garbage;
	bnode_cleanup(cur_n);
	cur_n = next_n;
    }
}

static int set_garbage (bnode_t *dst, bnode_t *src) {
    if(src == NULL || dst == NULL) {
	return -1;
    }
    if(dst->garbage == NULL) {		/* don't break the chain if set */
	dst->garbage = src->garbage;
	src->garbage = dst;
	return 0;
    }
    return -1;
}

static bnode_t *add_to_upper(bnode_t *node, bnode_t *upper_n) { /* # RBT_NUM_KEYS, RBT_EMPTY */
    /* this expects 'node' to have only its first key set
       and will merge node into upper_n.
       it expects upper_n has already been split_if_full
       so it returns an error if it can't find a free key element.
       it destroys 'node' on success
       -- and so expects node is NOT linked into the garbage chain. */

    /* return NULL on failure, upper_n on success */
    if(node == NULL || upper_n == NULL) {
	fprintf(stderr, "add_to_upper: node or upper_n NULL.\n");
	return NULL;
    }

    if(node->key[0] == RBT_EMPTY || node->key[1] != RBT_EMPTY) {
	fprintf(stderr, "add_to_upper: node->key[0] == RBT_EMPTY || node->key[1] != RBT_EMPTY\n");
	/* improperly prepared node, only key[0] should be non-empty. */
	return NULL;
    }

    bnode_t *swap_n = new_bnode(NULL);		/* no garbage collector on swap object */

    int i  =  0;
    int ni =  0;
    int ui =  0;
    int si = -1;

    for(;  i<RBT_NUM_KEYS;  i++ ) {

	if(node->key[ni] != RBT_EMPTY) {

	    if(upper_n->key[ui] != RBT_EMPTY) {
		if(node->key[ni] < upper_n->key[ui]) {
		    swap_n->key [i] = node->key [0];
		    swap_n->link[i] = node->link[0];
		    swap_n->link[i+1] = node->link[1];
		    /* defer copying node->link[ni+1]; */
		    si = i;	/* record where key was inserted */
		    ni++;
		}else{
		    swap_n->key[i]  = upper_n->key[ui];
		    swap_n->link[i] = upper_n->link[ui];
		    /* defer copying upper_n->link[ui+1]; */
		    ui++;
		}
	    }/* upper_n->key[ui] == RBT_EMPTY */
	    else{
		swap_n->key [i] = node->key [0];
		fprintf(stderr, "upper %lu\n", swap_n->key[i]);
		swap_n->link[i] = node->link[0];
		fprintf(stderr, "upper ln %p\n", swap_n->link[i]);
		/* defer copying node->link[ni+1]; */
		/* unless last key */
		si = i;		/* record where key was inserted */
		i++;
		break;
	    }
	}else{
	    swap_n->key [i] = upper_n->key [ui];
	    swap_n->link[i] = upper_n->link[ui];
	    /* defer copying upper_n->link[ui+1]; */
	    ui++;
	}
    }
    /* finish deferred actions */
    swap_n->link[i]  = upper_n->link[ui];
    fprintf(stderr, "add_to_upper: last link[%d]: %p\n", i+1, swap_n->link[i]);
    swap_n->link[si+1] = node->link[1];
    fprintf(stderr, "add_to_upper: link[%d]: after insert: %p\n", si+1, swap_n->link[si+1]);

    /* expect that key was successfully set, but if it wasn't return error */
    if(si == -1) {
	fprintf(stderr, "add_to_upper: si == -1\n");
	bnode_cleanup(swap_n);	/* this is why we deferred setting swap_n's garbage; */
	return NULL;
    }

    /* COPY SWAP TO UPPER_N */
    for(i=0; i<RBT_NUM_KEYS; i++) {
	upper_n->key [i] = swap_n->key [i];
	upper_n->link[i] = swap_n->link[i];
    }
    upper_n->link[i] = swap_n->link[i];	/* copy last link */


    /* CLEANUP UNUSED TEMP NODES */
    bnode_cleanup(node);	/* we deferred new_upper_n's garbg in split_if_full */
    bnode_cleanup(swap_n);


    return upper_n;
}

static bnode_t *split_if_full (bnode_t *node) { /* #define RBT_NUM_KEYS, RBT_EMPTY */
    						/* == node (if not split)
						    | new_upper_n (if split)
						    | NULL on err
						 */

    if(node == NULL) {			/* return NULL on err */
	return NULL;
    }
    if(node->key[RBT_NUM_KEYS-1] == RBT_EMPTY) {/* return same node, node isn't full */
	return node;
    }

    fprintf(stderr, "SPLIT FULL NODE...\n");

    bnode_t *new_upper_n= new_bnode(NULL);	/* defer garbage collection
						   this node will be destroyed 
						   by add_to_upper,
						   or we must set garbage later */
    bnode_t *split_n    = new_bnode(node);

    int mi              = RBT_NUM_KEYS/2+2;
    new_upper_n->key[0] = node->key[mi];
    node->key[mi]       = RBT_EMPTY;
    new_upper_n->link[0]= node;
    new_upper_n->link[1]= split_n;

    int i, si;
    for(i = mi+1, si = 0; i < RBT_NUM_KEYS; i++, si++) {
	split_n->key [si] = node->key [i];
	split_n->link[si] = node->link[i];
	node->key [i]     = RBT_EMPTY;
	node->link[i]     = RBT_EMPTY;
    }
    split_n->link[si]   = node->link[i];
    node->link[i]         = RBT_EMPTY; /* cleanup last link in node */

    return new_upper_n;
}

bnode_t *bnode_add(bnode_t *n, bkey_t key) {
    static int counter;
    if(n == NULL || key == RBT_EMPTY) return NULL;

    bkey_t next_key = RBT_EMPTY;
    int i;
    if(n->link[0] == NULL) {	/* indicates leaf */
	fprintf(stderr, "%d bnode_add: \n",counter++);

	for(i=0; i<RBT_NUM_KEYS; i++) {

	    if(n->key[i] == RBT_EMPTY) {
		n->key[i] = key;
		//fprintf(stderr, "bnode_add: key to leaf RBT_EMPTY\n");
		break;
	    }

	    if(key < n->key[i]) {
		next_key = n->key[i];
		n->key[i] = key;
		//fprintf(stderr, "bnode_add: key to leaf INSERT\n");
		key = next_key;
		continue;
	    }
	    if(key > n->key[i]) {
		//fprintf(stderr, "bnode_add: key to leaf SKIP\n");
		continue;
	    }


	    if(key == n->key[i]) {
		fprintf(stderr, "KEY EXISTS\n");
		i++;
		key = n->key[i];
		continue;
	    }
	}
	return n;
    }else{			/* branch indicated */

	for(i=0; i<RBT_NUM_KEYS; i++) {


	    if(key < n->key[i]) {
		fprintf(stderr, "%d < %d BRANCH INDICATED\n", key, n->key[i]);
		return n->link[i];
	    }
	    if(key == n->key[i]) {	/* key already exists */
		fprintf(stderr, "KEY EXISTS\n");
		return n;	/* treat as success   */
	    }
	    if(n->key[i] == RBT_EMPTY) {
		fprintf(stderr, "%d > %d BRANCH %p INDICATED\n", key, n->key[i-1],
								n->link[i]);
		return n->link[i];
	    }

	}
	return n->link[i];
    }
}


// bnode_t *btree_add(bnode_t *root_n, bkey_t key) {
int btree_put ( btree_t *t, bkey_t *key, bval_t *val, size_t valsize ) {
    //if(root_n == NULL || key == RBT_EMPTY) {
    if( t == NULL || key == RBT_EMPTY ) {

	/* fail silently, returning value of root_n */
	/* btree_add should be called like
	   "bnode_t *root_n = btree_add(root_n, key);" */
	return root_n;
    }

    bnode_t *cur_n   = root_n;
    bnode_t *upper_n = NULL;
    bnode_t *res_n   = NULL;
    bnode_t *new_root= root_n;

    do{
	res_n = split_if_full(cur_n);
	/* expect res_n == NULL if err,
	          res_n == cur_n if not split,
		  res_n != cur_n if split	*/

	/* ERROR ON SPLIT */
	if(res_n == NULL) {
	    fprintf(stderr, "btree_add: ERROR ON SPLIT\n");
	    /* fail silently, return value of cur root */
	    return new_root;
	}

	/* SPLIT */
	if(res_n != cur_n) {
	    /* upper_n might be NULL, in which case res_n
	       is indicated as new_root; */
	    if(upper_n == NULL) { /* requires previous fail if res_n == NULL */
		new_root = res_n;   
		/* res_n should be a result of split_if_full
		   and should NOT be connected to garbage */
		set_garbage(new_root, root_n);
		/* upper_n should remain null for now
		   but new_root should become current node */
		cur_n    = new_root;
		/* recurse loop */
		continue;
	    }
	    /* upper_n might be a node, in which case
	       res_n should be merged into upper_n
	       and the node pointed to by res_n should be destroyed.
	       at this point, res_n's node should be a product of split_if_full */
	    else {
		res_n = add_to_upper(res_n, upper_n);
		/* we can expect NULL on failure or upper_n on success */
		if(res_n != upper_n){
		    fprintf(stderr, "btree_add: adding to upper failed.\n");
		    /* if adding to upper failed, we have a problem,
		       if res_n isn't NULL and upper_n isn't NULL
		       we have to merge the nodes.  we can't fail gracefully.
		       we should return NULL,
		       and the caller of this function should abort on NULL
		       and cleanup the current tree using a garbage collector.
		     */
		    return NULL;
		}
		/* res_n == upper_n, success */
		else{
		    /* not necessary to re-assign, values should be same
		       however, makes a point  */
		    upper_n = res_n;
		    cur_n   = upper_n;  /* setup recursion */
		}
	    }
	}

	/* NOT SPLIT */
	if(res_n == cur_n) {
	    res_n = bnode_add(cur_n, key);
	    if(res_n == NULL) {	/* identical key exists */
		return new_root;/* fail silently, return value of cur root */
	    }
	    if(res_n == cur_n) { /* cur_n is leaf, successful add */
		return new_root; /* success, return value of cur root */
	    }
	    else{		 /* res_n is link, setup recursion: */
		upper_n = cur_n; /* current node becomes upper node */
		cur_n   = res_n; /* link becomes current node */
	    }
	}
    }while(cur_n != NULL);	/* expect always to evaluate true */

    /* just in case, fail silently, returning value of root_n */
    return root_n;
}



int main(int argc, char *argv[])
{
    int num_inserts = RBT_MAX_INSERTS;
    if(argc == 2) {
	num_inserts = atoi(argv[1]);
    }

   // bnode_t *root = new_bnode(NULL);
   // const bnode_t *garb = root; /* don't reset this */
    btree_t *t = btree_create();

    int i=0;
    char *key = NULL;
    while(i++ < num_inserts) {
        // root = btree_add(root, (ino_t)rand());
	// root = btree_add(root, i);
	snprintf( key, "%d", i );
	btree_put( t, key, NULL, 0 );

	if(t->root == NULL) {
	    fprintf(stderr, "root became null on %d\n",i);
	    break;
	}
    }

    fprintf(stderr, "original root: %p\n", t->garb);
    fprintf(stderr, "current root:  %p\n", t->root);

    printf("digraph \"b-tree test\" {\n");
    fflush(stdout);
    btree_cleanup( t );
    printf("}\n");
    fflush(stdout);
    fflush(stderr);
}
