#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ss.h"
#include "ppak.h"


/*== GLOBALS: parse packet ==*/
/* I don't know if this will optimise things? */
static const size_t sizeof_ppak = sizeof(Ppak);
/*--------*/



/*== OBJECT: list_t ==*/

list_t *new_list( list_t *garbage ) {
    list_t *cl = malloc(sizeof(*cl));
    if(cl != NULL) memset(cl, 0, sizeof(*cl));

    return cl;
}

list_t *new_list_t(void *ob) {
    list_t *lt = malloc(sizeof(*lt));
    if(lt == NULL) return NULL;
    memset(lt, 0, sizeof(*lt));
    if(ob != NULL) list_t_add_ob(lt, ob);
    return lt;
}

int list_t_cleanup(list_t *lt) {
    if(lt == NULL) return -1;
    free(lt);
    return 1;
}

int list_verify(list_t *cl) {
    if(cl == NULL) return -1;
    return 1;
}

list_t *list_get_next(list_t *cl) {
    if(cl == NULL) return NULL;

    return cl->next;
}
void list_set_ob(list_t *cl, void *ob) {
    if(cl == NULL) return;

    cl->ob = ob;
}
void *list_get_ob(list_t *cl) {
    if(cl == NULL) return NULL;

    return cl->ob;
}
int list_t_add_ob(list_t *lt, void *ob) { /* == TRUE | FALSE */
    if(lt == NULL) return -1;
    lt->ob = ob;

    return 1;
}

void *list_get_match ( list_t *cl, int (*ob_match_op)(void *, void *), void *val ) {
    list_t *cur_cl = cl;

    if(cl == NULL || ob_match_op == NULL) return NULL;

    while( cur_cl != NULL ) {
	if(ob_match_op(cur_cl->ob, val)) {
	    return cur_cl->ob;
	}
	cur_cl  = cur_cl->next;
    }
    return NULL;
}

void list_cleanup_all ( list_t *cl, void (*ob_cleanup_op)(void *)) {
    if(cl == NULL || ob_cleanup_op == NULL) return;

    list_t *cur_listl = cl;
    list_t *next_listl = NULL;

    while( cur_listl != NULL ) {
	ob_cleanup_op( cur_listl->ob );
	next_listl = cur_listl->next;
	list_t_cleanup(cur_listl);
	cur_listl = next_listl;
    }

    return;
}


int list_t_cleanup_garbage(list_t *lt) {
    list_t *nextlt;
    while(lt != NULL) {
	nextlt = lt->next;
	if(nextlt != NULL) nextlt->prev = NULL;
	list_t_cleanup(lt);
	lt = nextlt;
    }
    return 1;
}


/*--------*/



/*== FUNCTIONS: Object Map: positional node ==*/
/* defined up top here, because Ppak functions
 * rely on it.
 */

Ppposn *hiena_positional_node_create(off_t pos)
{
    Ppposn *posn = malloc(sizeof(Ppposn));
    if(posn == NULL) return NULL;
   
    memset(posn,0,sizeof(Ppposn));
    posn->pos = pos;

    return posn;
}

void hiena_positional_node_cleanup(Ppposn *posn)
{
    free(posn);
}

/*--------*/


/*== OBJECT: bounds_t ==*/

bounds_t *new_bounds ( int dim ) {
    bounds_t *b = malloc(sizeof(bounds_t));
    if(b == NULL)
	return NULL;
    memset(b,0,sizeof(bounds_t));


    if(dim < 1) dim = 1;

    b->a = malloc(sizeof(boundl)*dim);
    if(b->a == NULL) {
	free(b);
	return NULL;
    }
    memset(b->a,0,sizeof(boundl)*dim);

    return b;
}

void bounds_cleanup ( bounds_t *b ) { /* == TRUE OR FALSE */
    /* MEMORY: 'bounds' allocated by scanner, free'd by Ppak */
    if (b != NULL) {
	free(b->a);
	free(b);
    }
    return 1;
}

boundl *bounds_get_axis_ptr(bounds_t *b, int axis) {
    if(b == NULL || b->a == NULL) return NULL;

    int i;
    for(i = 0; i < b->c; i++) {
	if(b->a[i].axis == axis) {
	    return &b->a[i];
	}
    }
    return NULL;
}

int bounds_set_offset ( bounds_t *b, int axis, off_t offset ) {
    if(b == NULL) return -1;
    boundl *a = bounds_get_axis_ptr(b, axis);
    if(a == NULL) return -1;
    a->offset = offset;
    return 1;
}

int bounds_set_len ( bounds_t *b, int axis, off_t len ) {
    if(b == NULL) return -1;
    boundl *a = bounds_get_axis_ptr(b, axis);
    if(a == NULL) return -1;
    a->len = len;
    return 1;
}

off_t bounds_get_offset(bounds_t *b, int axis) {
    if(b == NULL) return 0;
    boundl *a = bounds_get_axis_ptr(b,axis);
    if(a == NULL) return 0;
    return a->offset;
}

off_t bounds_get_len(bounds_t *b, int axis) {
    if(b == NULL) return 0;
    boundl *a = bounds_get_axis_ptr(b,axis);
    if(a == NULL) return 0;
    return a->len;
}



int bounds_add_axis ( bounds_t *b, int axis, off_t offset, off_t len ) { /* ERROR | SUCCESS */
    if(b == NULL) {
	fprintf(stderr, "bounds_add_axis: bounds_t input NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }
    if(b->a == NULL) {
	/* this shouldn't bet the case if we used new_bounds() to create b */
	/* at this point we only need one array element */
	b->a = malloc(sizeof(boundl));
	if(b->a == NULL) {
	    fprintf(stderr, "bounds_add_axis: can't init b->a, abort.\n");
	    fflush(stderr);
	    return -1;
	}
	b->c = 1;
	b->a[0].axis   = axis;
	b->a[0].offset = offset;
	b->a[0].len    = len;
    }else{

	/* if b->a exists, 
	   we need to find if axis has already been registered */

	boundl *cur_bound = bounds_get_axis_ptr(b, axis);
	if(cur_bound != NULL) {
	    cur_bound->axis   = axis;
	    cur_bound->offset = offset;
	    cur_bound->len    = len;
	    goto bounds_element_is_set;
	}else{

	/* if bounds has not been registered, we need to add an element to the array */

	    /* first we try finding a '0' axis and write to that */

	    cur_bound = bounds_get_axis_ptr(b, 0);
	    if(cur_bound != NULL) {
		cur_bound->axis   = axis;
		cur_bound->offset = offset;
		cur_bound->len    = len;
		goto bounds_element_is_set;
	    }

	}
	
	/* if we didn't find a '0' axis we must extend the array */

	int newbounds_dim = b->c + 1;
	boundl *newbounds = malloc((sizeof *newbounds)*newbounds_dim);
	if(newbounds == NULL) {
	    fprintf(stderr, "ppak_set_bounds: malloc returned NULL, abort.\n");
	    fflush(stderr);
	    return -1;
	}

	boundl *res = memcpy(newbounds, b->a, sizeof(boundl)*b->c);
	if(res != newbounds) {
	    /* an error occured during memcpy */
	    fprintf(stderr, "ppak_set_bounds: memcpy returned bad value, abort.\n");
	    fflush(stderr);
	    return -1;
	}

	/* looks like memcpy worked, swap in new bounds, destroy old bounds */

	boundl *oldbounds = b->a;
	b->a = newbounds;
	free(oldbounds);

	/* set the new element */

	b->a[newbounds_dim-1].axis   = axis;
	b->a[newbounds_dim-1].offset = offset;
	b->a[newbounds_dim-1].len    = len;
	b->c = newbounds_dim;
    }
bounds_element_is_set:
    return 0;
}

/** COMPARING BOUNDARIES **/

/* A note:

   there are two cases when we need to compare bounds:
	1) assigning an object to a map node
	2) comparing objects

   for the assignment case, we are not concerned with 'len'
   we only need to know if 'axis' and 'offset' match
   if they both match we have an offset match on one axis.

   if we have an offset match on all axis, we have a point match between bounds.

   if we have an offset match on all axis, but "len"s differ we have an intersection.

   intersections are provided without a clear purpose,
   but with the thought that they may be useful to someone in the future.

   if we have a offset match on all axis AND a "len" match on all axis,
   we have a true 1:1 bounds match.

 */

int bounds_pointcpy(bounds_t *dest, bounds_t *src) { /* SUCCESS 1 | ERROR -1 */
    if(dest == NULL || src == NULL) {
	fprintf(stderr, "bounds_pointcpy: dest or src can't be NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }
    boundl *old_array = dest->a;

    dest->a = malloc(sizeof(boundl)*src->c);
    if(dest->a == NULL) {
	fprintf(stderr, "bounds_pointcpy: init dest->a == NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }

    if(memcpy(dest->a, src->a, sizeof(boundl)*src->c) == NULL) {
	fprintf(stderr, "bounds_pointcpy: can't memcpy, abort.\n");
	    fflush(stderr);
	return -1;
    }
    dest->c = src->c;
    if(old_array != NULL)
	free(old_array);

    /* erase 'len' component of dest */
    /* (this is a point, not a box) */
    
    int i;
    for(i=0; i < dest->c; i++) {
	dest->a[i].len = 0;
    }

    return 1;
}

bounds_t *bounds_cmp ( bounds_t *subject, bounds_t *control, bounds_t *result ) {

    /* This function compares two bounds_t objects
       and spits out a new bounds_t object
       -- unless "result" not NULL then store result in "result"
       whose internal array elements contain offsets
       from control to subject,
       whose 'pointmatch' is set if subject and control have identical
       'offset' values on all axis,
       whose 'status' flag is set to indicate the type of intersection
       between the full bounds of subject and control.
     */

    if(subject == NULL || control == NULL) {
	fprintf (stderr, "bounds_cmp: subject or control can't be NULL, abort.\n");
	    fflush(stderr);
	return NULL;
    }

    if(result == NULL) result = new_bounds(1);
    if(result == NULL) {
	fprintf (stderr, "bounds_cmp: can't init result object, abort.\n");
	    fflush(stderr);
	return NULL;
    }

    /* step through all axis in subject
       and compare each to control's */

    int i;
    boundl   *ap;	/* axis object pointer */
    boundl   *aps;	/* axis object pointer from subject */
    for(i = 0; i < subject->c; i++) {
	
	/* match axis from subject to control */

	aps = &subject->a[i];

	/* make sure axis is positive */

	if(aps->axis < 1) {
	    fprintf(stderr, "bounds_cmp: subject's axis less than 1, abort.\n");
	    fflush(stderr);
	    goto cleanup_and_abort;
	}
	ap = bounds_get_axis_ptr(control, aps->axis);


	if(ap == NULL) {

	/* control doesn't have same axis */
	/* add result axis with negated value */

	    bounds_add_axis(result, -(aps->axis), 0, 0);

	}else{

	/* control has the same axis */
	/* make offset array element */

	    bounds_add_axis(result,
		    aps->axis,
		    aps->offset - ap->offset,
		    aps->len - ap->len);

	
	    /* using the result from above, calculate pointmatch */
	    /* reminder: this may evaluate during every loop iteration */

	    ap = bounds_get_axis_ptr(result, aps->axis);
	    if(ap->offset == 0) {

		/* offsets match */

		if(result->pointmatch == 0) { 	/* 0 if no matching performed yet */
		    result->pointmatch = 1;	/* 1 b/c all offsets have matched so far */
		}
	    }else{

		/* offsets don't match */

		result->pointmatch = 2;	/* 2 if one or more offsets have not matched */
	    }
	
	    /* calculate boundsmatch */

	    if(ap->len == 0) {
		
		/* exact 'len' match */

		if(result->status == 0)  /* 0 = status hasn't been set yet */
		    result->status = BNDS_SAME;
	    }else{

		/* len's don't match */

		result->status = BNDS_DIFF;
	    }
	}
    }
    return result;
cleanup_and_abort:
    bounds_cleanup(result);
    return NULL;


    /* RESULTS FORMAT */
    /*
       bounds_t
         := count c
	    intersection status
	    array of boundl

       array of boundl
         := ( subject is offset from control on axis A by offset F )+

       subject is offset from control on axis A by offset F
         := axis A, offset F, len 0
	 
       intersection status
         := subject is within control bounds
	 |  subject is not within control bounds
	 |  control is within subject bounds
	 |  subject intersects control bounds

       subject is within control bounds
         := 1

       subject is not within control bounds
         := 2

       control is within subject bounds
         := 3

       subject intersects control bounds
         := 4
     */
}


/*--------*/


/*== OBJECT: dscell_t ==*/
dscell_t *new_dscell () {
    dscell_t *c = malloc(sizeof(*c));
    if(c != NULL)
	memset(c,0,sizeof(*c));
    return c;
}

int dscell_set_redo ( dscell_t *c, dscell_t *rc ) {
    if(c == NULL) return -1;
    return c->redo = rc, 0;
}

int dscell_set_undo ( dscell_t *c, dscell_t *uc ) {
    if(c == NULL) return -1;
    c->undo = uc;
    /* implicitly sets redo */
    return dscell_set_redo(uc, c);
}

dscell_t *dscell_get_next ( dscell_t *c ) {
    if(c == NULL) return NULL;
    return c->next_cell;
}

bounds_t *dscell_get_bounds ( dscell_t *c ) {
    if(c == NULL) return NULL;
    return c->bounds;
}

void dscell_cleanup_chain( dscell_t * );
void dscell_cleanup_undochain( dscell_t * );

void dscell_cleanup ( dscell_t *c ) {
    if(c == NULL)
	return;

    /* a complex task:
       for each cell in c.cell[*]	{ dscell_cleanup_chain }
           cleanup each
       for each frag in c.frag[*]	{ frag_cleanup_chain }
           cleanup frag
       cleanup undo chain		{ dscell_cleanup_undochain }
    */
    if(c->first_cell != NULL) {
	dscell_cleanup_chain( c->first_cell );
    }
    if(c->first_frag != NULL) {
	frag_cleanup_chain( c->first_frag );
    }
    dscell_cleanup_undochain( c );
    bounds_cleanup( dscell_get_bounds(c) );
    free(c);
    c = NULL;
}

void dscell_cleanup_chain ( dscell_t *c ) {
    dscell_t *cur_c = c;
    dscell_t *next_c = NULL;
    while(cur_c != NULL) {
	next_c = dscell_get_next(cur_c);
	dscell_cleanup(cur_c);
	cur_c = next_c;
    }
}

void dscell_cleanup_undochain ( dscell_t *c ) {
    dscell_t *cur_state = c;
    dscell_t *undo_state = NULL;
    if(c != NULL)
	dscell_t *redo_state = dscell_get_redo(cur_state);

    while(cur_state != NULL) {
	undo_state = dscell_get_undo(cur_state);
	dscell_cleanup(cur_state);
	cur_state = undo_state;
    }
    cur_state = redo_state;
    while(cur_state != NULL) {
	redo_state = dscell_get_redo(cur_state);
	dscell_cleanup(cur_state);
	cur_state = redo_state;
    }
}

void dscell_cleanup_wrapper ( dscell_t *c ) {
    /* only cleans up the cell wrapper itself
       leaves the contents to be free'd elsewhere */
    if(c == NULL) return;
    bounds_cleanup(c->bounds);
    free(c);
    c = NULL;
}

int dscell_init_frag ( dscell_t *c, frag_t *f ) {
    if(c == NULL || f == NULL) return;
    if(c->first_frag == NULL && c->last_frag == NULL) {
    /* a condition that we only init a cell whose frag pointers are NULL */

	c->first_frag = f;
	c->last_frag  = f;
	return 0;
    } else {
	fprintf(stderr, "hiena: dscell_init_frag: first or last frag NOT NULL, abort.\n");
	return -1;
    }
}

dscell_t *dscell_dupe( dscell_t *c ) {
    dscell_t *dupc = new_dscell();
    if(dupc != NULL)
        memcpy(dupc, c, sizeof(*c));
    return dupc;
}

/* WORKING */
void dscell_insert_frag ( dscell_t *c, frag_t *f, bounds_t *point ) {
}


dscell_t *dscell_carve_at_bounds ( dpakstream *ds, bounds_t *b ) {
    /* CREATE NEW CELL WITHIN STREAM
       BY ENCAPSULATING EVERYTHING INSIDE BOUNDS */
    /* SPLIT EXISTING CELLS AT BOUNDS IF NECESSARY */
    return NULL;
}

dscell_t *dscell_carve_around_bounds ( dpakstream *ds, bounds_t *b ) {
    return NULL;
}

dscell_t *dscell_carve_around_point ( dpakstream *ds, bounds_t *point ) {
    /* CREATE NEW CELL WITHIN STREAM
       BY ENCAPSULATING ANY CELLS SURROUNDING POINT */
    /* NO CELL SPLITTING */
    return NULL;
}
/*--------*/



/*== OBJECT: mapnode ==*/
/* used by new_mapnode */
int mapnode_add_ob(Mapn *mn, Ppak *ob) { /* == TRUE | FALSE */

    if(mn == NULL || ob == NULL){
	fprintf(stderr, "mapnode_add_ob: mapnode and object can't be NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }

    list_t *newob = new_list_t(ob);
    if(newob != NULL) {
	newob->next = mn->ob;
	mn->ob = newob;
    }

    return 1;
}




int mapnode_cleanup(Mapn *mn) { /* == TRUE OR FALSE */
    if(mn == NULL) return 1;

    /* remove this link from the garbage chain */
    if(mn->prev_garbage != NULL) {
	mn->prev_garbage->garbage = mn->garbage;
    }

    /* cleanup bounds */
    bounds_cleanup(mn->bounds);

    /* cleanup object list */
    list_t_cleanup_garbage(mn->ob);

    /* cleanup mapnode connections list */
    list_t_cleanup_garbage(mn->con);

    free(mn);
    mn = NULL;


    return 1;
}

int mapnode_cleanup_garbage(Mapn *garbage) { /* == TRUE OR FALSE */
    Mapn *nextgarb;
    while(garbage != NULL) {
	nextgarb = garbage->garbage;
	mapnode_cleanup(garbage);
	garbage = nextgarb;
    }
    return 1;
}
Mapn *new_mapnode(Ppak *ob, Mapn *garbage_can){

    Mapn *mn = malloc(sizeof(Mapn));
    if(mn == NULL) {
	fprintf (stderr, "new_mapnode: can't malloc, abort.\n");
	    fflush(stderr);
	return NULL;
    }
    memset(mn, 0, sizeof(Mapn));

    /* init bounds object */

    mn->bounds = new_bounds(1);
    if(mn->bounds == NULL) {
	fprintf (stderr, "new_mapnode: can't init bounds, abort.\n");
	    fflush(stderr);
	goto cleanup_and_abort;
    }

    /* copy point information from ob->addr->bounds */
    if(ob != NULL) {
	if(ob->addr != NULL && ob->addr->bounds != NULL) {
	    bounds_pointcpy(mn->bounds, ob->addr->bounds);
	    //mapnode_add_ob(mn, ob);
	}else{
	    fprintf(stderr, "new_mapnode: non-null ob, can't have null bounds, abort.\n");
	    fflush(stderr);
	    goto cleanup_and_abort;
	}
    }

    /* insert this link into garbage chain */
    if(garbage_can != NULL) {
	mn->prev_garbage = garbage_can;
	mn->garbage = garbage_can->garbage;
	garbage_can->garbage = mn;
    }
    fprintf(stderr, "new_mapnode: initialized; ");
	    fflush(stderr);

    return mn;
cleanup_and_abort:
    mapnode_cleanup(mn);
    return NULL;

}
/*--------*/


/*== OBJECT: dpak ==*/
/* aka ( OBJECT: ppak ) */
/* aka ( OBJECT: Dpak ) */
/* aka ( OBJECT: Ppak ) */

Ppak *new_ppak(Ppak *garbage_can) {

    /* currently, July 6, 2015, this takes a Ppak * on the stack
       which it uses for garbage collection.
       currently, ditto, ditto, this will over-free map objects
       and is only necessary for creating non-mapped objects
       like union members, derivatives, alternatives, undo states, etc.
       mapped objects have their own garbage collection
       and CURRENTLY the two garbage collection methods are INCOMPATIBLE.
       if it's in the map, initialize it with a NULL garbage_can.
       - Raygan
       */


    Ppak *pp = malloc(sizeof_ppak);
    if(pp == NULL) {
	return NULL;
    }
    memset(pp,0,sizeof_ppak);

    if(garbage_can != NULL){
	pp->garbage_prev = garbage_can->garbage_prev;	/* setup garbage can */
	pp->garbage_next = garbage_can;
	garbage_can->garbage_prev = pp;
    }
    return pp;
}
Dpak *new_dpak( Dpak *garbage_can ) {
    return new_ppak( garbage_can );
}




void ppak_cleanup(Ppak *pp) {
    /* cleans up a single parse packet and its data */
    /* does not ripple-delete other packets */
    if(pp == NULL)	return;

    if(pp->retain > 0) {
	pp->retain--;
	return;
    }

    /* cleanup sub domain */
    /*if(pp->val.parstream!=NULL) {
	fclose(pp->val.parstream);
    }
    */


    /* CLEANUP VAL STREAM */
    frag_cleanup_chain(pp->val.first_frag);

    /* <--- release server here... */


    /* CLEANUP PROPERTY MAP */
    mapnode_cleanup_garbage(pp->prop_root);

    /* CLEANUP CHILD MAP */
    mapnode_cleanup_garbage(pp->child_root);

    /* TBD */

    /* CLEANUP ADDRESS PROPERTY */
    hpat_cleanup(pp->key.tokname);
    hpat_cleanup(pp->key.tokfull);
    addr_t_cleanup(pp->addr);
    /* 2015-11-13 MOVED 'bounds' into 'addr' */

    //hiena_parse_packet_map_cleanup((void *)pp);

    /* REMOVE FROM GARBAGE FACILITY */
    if(pp->retain==0) {
	if(pp->garbage_prev != NULL) {
	    pp->garbage_prev->garbage_next = pp->garbage_next;
	} else if(pp->garbage_next != NULL) {
	    pp->garbage_next->garbage_prev = NULL;
	}

	free(pp);

	pp = NULL;
    }
}

void ppak_cleanup_garbage(Ppak *garbage_can) {
    Ppak *choice_garbage;
    if(garbage_can == NULL) {
	fprintf(stderr, "trying to empty an empty garbage can, returning.\n");
	    fflush(stderr);
	return;
    }
    while(garbage_can->garbage_prev != NULL) {
	choice_garbage = garbage_can;
	garbage_can    = garbage_can->garbage_prev;
	garbage_can->garbage_next = choice_garbage->garbage_next;
	choice_garbage->retain = 0;
	ppak_cleanup(choice_garbage);
    }
    while(garbage_can->garbage_next!=NULL) {
	choice_garbage = garbage_can;
	garbage_can    = garbage_can->garbage_next;
	garbage_can->garbage_prev = NULL;
	choice_garbage->retain = 0;
	ppak_cleanup(choice_garbage);
    }
    garbage_can->retain = 0;
    ppak_cleanup(garbage_can);
}

/************************ DPAK ACCESSOR: HOUSEKEEPING *****************************/

void ppak_retain ( Dpak *p ) {
    if(p != NULL) p->retain++;
}
void ppak_release ( Dpak *p ) {
    if(p != NULL) p->retain--;
}
void ppak_incr_opencount ( Dpak *p ) {
    if(p != NULL) p->opencount++;
}
void ppak_decr_opencount ( Dpak *p ) {
    if(p != NULL) p->opencount--;
}
void ppak_set_mode ( Dpak *p, const char *mode ) {
    return p == NULL ? NULL : p->mode;
}
const char *ppak_get_mode ( Dpak *p ) {
    return p == NULL ? NULL : p->mode;
}
void ppak_set_error ( Dpak *p, dpakstream_error_t err ) {
    if(p != NULL) p->error = err;
}
dpakstream_error_t ppak_get_error ( Dpak *p ) {
    return p == NULL ? NULL : p->error;
}
    



/**************************** DPAK ACCESSOR KEY *********************************/

int ppak_import_key(Ppak *pp, Ppkey *key) {
    pp->key.scanner        = key->scanner;
    pp->key.tokid         = key->tokid;
    pp->key.tokname        = hpat_dupe(key->tokname);
    pp->key.tokfull        = hpat_dupe(key->tokfull);

    return 0;
}


/**************************** DPAK ACCESSOR ADDR *********************************/
int pak_set_addr_str (Ppak *p, const char *str) { /* TRUE | FALSE */
    /*
       p  := PPAK   && !NULL
           ;
       str := STRING | NULL
           ;
     */
    if(p == NULL) return 0;

    if(p->addr == NULL) {
	p->addr = new_addr_t();
	if(p->addr == NULL)
	    return -1;
    }

    hpat_cleanup(p->addr->pat);
    p->addr->pat = new_hpat_from_str(str);

    return 0;
}
int ppak_set_addr_par ( Ppak *pp, Ppak *par ) { /* 0 | ERROR */
    if(pp == NULL || par == NULL) return;
    if(pp->addr == NULL) {
	pp->addr = new_addr_t();
	if(pp->addr == NULL)
	    return -1;
    }
    pp->addr->par = par;
    return 0;
}
int ppak_set_addr_ptr ( Dpak *p, void *ptr, size_t size ) { /* TRUE | FALSE */
    if( p == NULL || p->addr == NULL ) return 0;
    p->addr->ptr = ptr;
    p->addr->size = size;
    return 1;
}

int dpak_set_addr_strn ( Dpak *p, const char *str, size_t len ) { /* 1 | 0 */
    if( p == NULL || p->addr == NULL ) return 0;
    p->addr->ptr = (void *)str;
    p->addr->size = len;
    return 1;
}

addr_t *ppak_get_addr( Dpak *p ) {
    if(p == NULL) return NULL;
    return p->addr;
}
int ppak_verify_addr(Hpat *addr) { /* := TRUE | FALSE */	/* TODO: replace Hpat with addr_t */
    if(addr == NULL) return 0;

    if(hpat_is_c_string(addr))
    {
	return 1;
    }
    return 0;
}


/**************************** DPAK ACCESSOR SERVER *********************************/
int dpak_set_server_sign ( Dpak *p, char *str, size_t len ) { /* 1 | 0 */
    if( p == NULL || str == NULL || len == 0 ) return 0;
    size_t i;
    for( i = 0; i <= len; i++ ) {
	p->serversign[i] = str[i];
    }
    return 1;
}
int ppak_set_server(Ppak *p, dpakserver *sv) { /* TRUE | FALSE */
    if(p == NULL) {
	fprintf(stderr, "ppak_set_server: Ppak can't be NULL, abort.\n");
	    fflush(stderr);
	return 0;
    }
    p->server = sv;
    return 1;
}

int ppak_set_server_name ( Dpak *p, const char *name, list_t *servers ) { /* TRUE | FALSE */
    if(p == NULL) { return 0; }
    return p->server = list_get_match( servers, server_name_match_op, name ) != NULL ? 1 : 0;
}

dpakserver *ppak_get_server ( Dpak *p ) {
    if(p == NULL) return NULL;
    return p->server;
}
int ppak_verify_server(struct hiena_serverops *server) { /* := TRUE | FALSE */
    if(server == NULL)		return 0; 
    if(server->fopen == NULL)	return 0;
    return 1;
}



/************ MAP ACCESSORS *************************************/


list_t *ppak_get_first_prop ( Ppak *p ) {
    if(p == NULL) return NULL;

    return p->first_prop;
}

list_t *ppak_get_next_prop ( Ppak *p, list_t *cur_prop ) {
    if(p == NULL || cur_prop == NULL) return NULL;
    /* I admit this looks funny.  Like p isn't used.
       but I have it here - one to make the caller believe that
       cur_prop must be a part of p.  And two,
       in case the method changes in the future. */

    return cur_prop->next;
}

list_t *ppak_get_first_child ( Ppak *p ) {
    if(p == NULL) return NULL;

    return p->first_child;
}

list_t *ppak_get_next_child ( Ppak *p, list_t *cur_child ) {
    if(p == NULL || cur_child == NULL) return NULL;

    return cur_child->next;
}

list_t *get_next_row(Dpak *cur_tab, list_t *cur_row) {
    if(cur_tab == NULL) return NULL;

    if(cur_row == NULL)
	return ppak_get_first_child(cur_tab);

    if(list_verify(cur_row))
	return list_get_next(cur_row);

    return NULL;
}

list_t *ppak_get_next_col ( Dpak *cur_tab, list_t *cur_col ) {
    if(cur_tab == NULL) return NULL;

    if(cur_col == NULL)
	return ppak_get_first_prop(cur_tab);

    if(list_verify(cur_col))
	return list_get_next(cur_col);

    return NULL;
}






/************ IMPORT VALUE FROM ANOTHER PARSE PACKET ************/
int
ppak_import_val(Ppak *pp, Ppval *val) {
    pp->val.inparent.offset  = val->inparent.offset;
    pp->val.inparent.len     = val->inparent.len;
    pp->val.is_parent_backed = val->is_parent_backed;
    pp->val.buf              = hpat_dupe(val->buf);

    return 0;
}
int
hiena_parse_packet_import_val(Ppak *pp, Ppval *val) {
    return ppak_import_val(pp,val);
}




/********** CREATOR-INITIALIZERS *************/

/* RE-WRITING with frag buffers instead of Hpat's */


Ppak *
new_ppak_with_hpat(Hpat *pat, Ppak *gcan) { /* NULL ON ERROR */
    /* pat == NULL | !NULL */

    Ppak *pak = new_ppak(gcan);
    if(pak == NULL)
	return NULL;
    
    if(pat!=NULL) {
	int errno = ppak_set_buf(pak,pat);
	if(errno == -1) {
	    fprintf(stderr,"libhiena: ppak: trouble setting buffer from hpat.\n");
	    fflush(stderr);
	    goto abort;
	}
        return pak;
    }else{
	return pak;
    }
abort:
    ppak_cleanup(pak);
    return NULL;
}

Ppak *new_ppak_from_strv(int argc, char *argv[], Ppak *gcan) {
    Hpat *pat = new_hpat_from_argv(argc, argv);
    if(pat == NULL) return NULL;
    return new_ppak_with_hpat(pat,gcan);
}

Ppak *new_ppak_from_str(const char *str, Ppak *gcan)
{
    Hpat *pat = new_hpat_from_string(str);
    /* the following is ok if pat == NULL */
    return new_ppak_with_hpat(pat,gcan);
}

Ppak *new_ppak_with_addr_str(const char *str, Ppak *gcan)
{
    /* str  := STRING | NULL;
       gcan := PPAK   | NULL;
     */
    Ppak *p  = new_ppak(gcan);
    if(p == NULL)
	return NULL;

    ppak_set_addr_str(p, str);

    return p;
}

Dpak *new_dpak_with_addr_str( const char *str, size_t *len, Dpak *gcan ) {
    if( str == NULL ) return NULL;

    Dpak *p = new_dpak( gcan );
    if( p == NULL) return NULL;

    char *str2 = strndup( str, len );
    if( str2 == NULL ) goto abort;

    if( ! dpak_set_addr_strn( p, str2, len ) ) goto abort2;

    /* auto detect address string */

    /* prioritize existing files to override all URL expressions */

    /* first look in working directory for complete URL str as filename */
    if( access( "str2", F_OK ) != -1 ) {
    /* if filename is found, set "file" type */
	dpak_set_server_sign( p, "file", 4 ); 
    } else {
	perror( "new_dpak_with_addr_str" );
	goto abort;
    
    /*** NEXT VERSION: TDB  ***/
    /* if filename is not found, detect server type from 'protocol'
       rule member of URL */
    }
    
    return p;

abort2:

    free( str2 );

abort:

    dpak_cleanup( p );
    return NULL;

}


/******** DPAK TESTS **********/

int ppak_is_dud ( Dpak *p ) { /* TRUE | FALSE */
    if(p == NULL) return 0; /* p doesn't exist, therefore can't be a "dud" */

    /* 
       2015-11-13 version determines a "dud" by the lack of an 'addr'
     */

    if(p->addr == NULL)
	return 1;
    return 0;
}



/************* SET VALUE AREA IN PARENT ************************/
int
ppak_set_val_in_parent(Ppak *pp, uint64_t offset, uint64_t len)
{
    if(pp == NULL) return -1;
    pp->val.inparent.offset = offset;
    pp->val.inparent.len    = len;
    pp->val.is_parent_backed = 1;
    return 0;
}


void ppak_set_filter ( Dpak *p, void *filter ) {
    if(p == NULL) return;
    p->filter = filter;
}

int ppak_set_filter_name ( Dpak *p, const char *name, scanlib *slib ) { /* TRUE | FALSE */
    if(p == NULL || name == NULL) return 0;
    return p->filter = slib_get_scanner(slib, name) != NULL ? 1 : 0 ;
}
scannerserver *ppak_get_filter ( Dpak *p ) {
    if(p == NULL) return NULL;
    return p->filter;
}

frag_t *ppak_get_first_frag ( Dpak *p ) {
    if(p == NULL) return NULL;
    return p->val.first_frag;
}





/*== FUNCTIONS: sql-like abstract api ==*/


#define H_ATAB 1	/* analytical table id */
#define H_CTAB 2	/* child table id */

Ppak *
hiena_select_all_from_table_where_key_like(int tableid, char *keyid, char *matchexp);

Ppak *
hiena_select_all_from_table_where_key_is(int tableid, char *keyid, char *matchexp);

/*--------*/



/*== FUNCTIONS: map api (abstract object) ==*/

/* see more in "atab.c" */
/* try to deprecate "atab.c" */
Ppposn *
hiena_get_posnode(Ppak *src, Ppbounds *bounds) {

    if(src==NULL || bounds==NULL)
	return NULL;

    Ppposn *cur_posnode = src->first_posnode;
    Ppposn *prev_posnode = NULL;

    if(cur_posnode==NULL){
	src->first_posnode = hiena_positional_node_create(bounds->offset);
	return src->first_posnode;
    }

    while(cur_posnode->pos < bounds->offset) {
	prev_posnode = cur_posnode;
	cur_posnode  = cur_posnode->next_posnode;
	if(cur_posnode==NULL){
	    prev_posnode->next_posnode = hiena_positional_node_create(bounds->offset);
	    return prev_posnode->next_posnode;
	}
    }
    if(cur_posnode->pos == bounds->offset)
	printf("found existing posnode\n");
	    fflush(stderr);
	return cur_posnode;
    if(cur_posnode->pos > bounds->offset) {
	prev_posnode->next_posnode = hiena_positional_node_create(bounds->offset);
	prev_posnode->next_posnode->next_posnode = cur_posnode;
	return prev_posnode->next_posnode;
    }
	
    return NULL;
}

int
hiena_posnode_add_object(Ppposn *posn, Ppak *newob) {
    if(posn==NULL || newob==NULL)
	return -1;

    newob->next_aligned = posn->first_aligned;
    posn->first_aligned = newob;

    return 0;
}

Ppak *
hiena_posnode_get_token(Ppposn *posn, Ppkey *key) {
    if(posn==NULL || key==NULL)
	return NULL;

    Ppak *cur_ob = posn->first_aligned;
    while(cur_ob!=NULL){
	if(cur_ob->key.scanner == key->scanner)
	    if(cur_ob->key.tokid     == key->tokid)
		return cur_ob;
	cur_ob = cur_ob->next_aligned;
    }
    return NULL;
}




/*--------*/



/*== OBJECT: FILE* interface ==*/

static FILE *ppak_parent_fopen(Ppbounds *loc, const char *mode) {
    /** this will look in parent ppak and extract an area from the buffer
      or "ripple" up the tree of parents
      and return a FILE pointer to a fragment which holds the data.
     **/
    printf("ppak_parent_fopen: does nothing, return NULL\n");
	    fflush(stderr);
    return NULL;
}


static FILE *ppak_server_fopen(Ppak *p, const char *mode) {
    if(p == NULL || mode == NULL) return NULL;

    if(ppak_verify_server(p->server))
    /* verifies server ops, and requires "fopen" */
    {
	if(ppak_verify_addr(p->addr->pat))
	/* verifies address is '\0' terminated */
	{
	    /*** TODO: validate 'mode' param to server->fopen ***/
	    char *s = hpat_str_ptr(p->addr->pat);
	    fprintf(stderr, "ppak_server_fopen: addr %s, mode %s\n", s, mode);
	    return p->server->fopen(p->addr->pat, mode);
	}
    }
    return NULL;
}

FILE *ppak_fopen(Ppak *p, char *mode) {
    if(p == NULL || mode == NULL) return NULL;

    /* possible stream conditions for "p":

       server,addr provides source

       empty server,addr; full value buffer provides source

       empty server, present 

       */

    if(ppak_verify_val(p))
    {
	if(p->val.buf == NULL)
	{
	    if(p->val.is_parent_backed)
	    {
		return ppak_parent_fopen(&p->val.location, mode);
	    }else{
		/* must get data from server */
		/* if we called ppak_verify_val()
		   we can assume there is an address and server */
		/*
		   we have two options:
		      1) fill buffer from address using server
		      2) serve directly, from server
			  ppak_server_fopen(Ppak *p, char *mode);

		   for now, option 2
		 */
		fprintf(stderr, "ppak_fopen: calling ppak_server_fopen()\n");
		return ppak_server_fopen(p, mode);
	    }
	}
	/* re-evaluate val.buf, probably not NULL now */
	if(p->val.buf != NULL)
	{
	    /* TODO: ppak_fopen: check for "dirty" before opening buffer */
	    fprintf(stderr, "ppak_fopen: calling hpat_fopen()\n");
	    return hpat_fopen(p->val.buf, mode);
	}
    }
    return NULL;
}

/*--------*/

/*== OBJECT: frag_t ==*/

frag_t *new_frag() {
    frag_t *f = malloc(sizeof(*f));
    if(f != NULL) memset(f,0,sizeof(*f));
    return f;
}

void frag_set_prev ( frag_t *f, frag_t *prev ) {
    if(f == NULL) return;
    f->prev = prev;
}
void frag_set_next ( frag_t *f, frag_t *next ) {
    if(f == NULL) return;
    f->next = next;
}
void frag_set_bounds ( frag_t *f, bounds_t *bounds ) {
    if(f == NULL) return;
    if(f->bounds != NULL) {
	bounds_cleanup(f->bounds);
    }
    f->bounds = bounds;
}
int frag_set_buf ( frag_t *f, void *buf, size_t *size ) { /* 0 | ERROR */
    if(f == NULL) return -1;

    if(f->buf != NULL) {
	if(f->buf_cleanup != NULL) {
	    f->buf_cleanup(f->buf);
	}else{
	    free(f->buf);
	    f->buf_cleanup = NULL;
	}
    }
    f->buf  = buf;
    f->size = size;
    return 0;
}
int frag_set_buf_cleanup ( frag_t *f, void (*buf_cleanup)(void *)) { /* 0 | ERROR */
    if(f == NULL) return -1;
    f->buf_cleanup = buf_cleanup;
    return 0;
}

frag_t *frag_get_prev ( frag_t *f ) {
    if(f == NULL) return NULL;
    return f->prev;
}
frag_t *frag_get_next ( frag_t *f ) {
    if(f == NULL) return NULL;
    if(f->next != NULL && f->next_cell == NULL) {
	return f->next;
    }
    if(f->next == NULL && f->next_cell != NULL) {
	return dscell_get_first_frag( f->next_cell );
    }
    fprintf(stderr, "hiena: frag_get_next: next cell and next frag must be exclusively set.\n");
    return NULL;
}
bounds_t *frag_get_bounds ( frag_t *f ) {
    if(f == NULL) return NULL;
    return f->bounds;
}
void   *frag_get_buf ( frag_t *f ) {
    if(f == NULL) return NULL;
    return f->buf;
}

void frag_cleanup ( frag_t *f ) {
    if(f == NULL) return;
    bounds_cleanup(f->bounds);
    if(f->buf_cleanup != NULL) {
	f->buf_cleanup(f->buf);
    }else{
	free(f->buf);			/* buf will be free'd if no custom cleanup */
    }
}

void frag_cleanup_chain ( frag_t *f ) {
    frag_t *cur_frag  = f;
    frag_t *next_frag = NULL;
    while(cur_frag != NULL) {
	next_frag = frag_get_next(cur_frag);
	frag_cleanup(cur_frag);
	cur_frag = next_frag;
    }
}

/*--------*/

/*== INLINE INCLUDE: dpakstream ==*/
#include "dpakstream.c"
/*--------*/

/************** MAPPING FUNCTIONS ******************/
/* NOTES:

   GARBAGE COLLECTION
   ==================
   Hsp must provide a garbage collector.
   These functions, when they create a Ppak must use Hsp's garbage facility.
   The simplest way to implement this is through a Garbage Bin ppak
   in Hsp which can be passed to a Ppak's init method.


   UNIQUE SCANNER SIGNATURE
   ========================
   Hsp contains information about the scanner which is generating this ppak.
   Take note: the 'tokid' passed to this function is scanner local.
   It will be used to generate the global scanner id from the scanner library.


   FLEXIBLE TYPE 'BOUNDS' OBJECT
   =============================
   The scanner generates a 'bounds' expression when it locates an object
   within its input stream.  The 'bounds' expression can be used to extract
   the object at a future time from the stream by the scannerserver.

   a hypothetical call might look like:

   	serverop->extract_b(void *bounds, FILE *stream);


   OBJECT BUFFER
   =============
   In the case that the input is a Pipe, FIFO or another ephemeral form,
   the scanner author may want to save a buffer -- or the semantic content
   will be lost.  The 'bounds' expression must also be used to extract
   the object from the buffer.

   a hypothetical call might look like:

       serverop->exbuf_b(void *bounds, void *buf);


   OBJECT SERVER
   =============
   A scanner gets to choose whence the data will be served from.  It can select
   from one of the built-in servers, or provide its own.


   OBJECT ADDRESS
   ==============
   In the case that the object is a reference to an address outside of the stream,
   or an address to another area within the stream, the scanner can set an address
   which will be compatable with the server.


   PARENT STREAM OBJECT
   ====================
   The Parent Stream is where the 'bounds' map to.
   The Hsp keeps track of the source object which the scanner is mapping from.
   Hiena will assign the source object to the map object's parent reference property.
   The scanner needn't worry itself with that detail.  It is implicit in the callback
   operations.



   */


int ppak_set_bounds(Ppak *p, int axis, off_t offset, off_t len) {

    if (p == NULL) {
	fprintf(stderr, "ppak_set_bounds: Ppak can't be NULL, abort.\n");
	fflush(stderr);
	return -1;
    }

    if (p->addr == NULL) {
	p->addr = new_addr_t();
	if(p->addr == NULL)
	    fprintf(stderr, "ppak_set_bounds: p->addr==NULL, abort.\n");
	    fflush(stderr);
	    return -1;
    }
    if (p->addr->bounds == NULL) {
	p->addr->bounds = new_bounds(1);
	if(p->addr->bounds == NULL) {
	    fprintf(stderr, "ppak_set_bounds: new_bounds() fail, abort.\n");
	    fflush(stderr);
	    return -1;
	}
    }
    fprintf(stderr, "ppak_set_bounds: axis %c, offset %lu, len %lu;  ", axis, offset, len); 
    fflush(stderr);
    return bounds_add_axis(p->addr->bounds, axis, offset, len);
}

void ppak_set_map_buf(Ppak *p, off_t len, void *buf) {
    
    len++;
    void *bufcpy = malloc(len);
    if(bufcpy == NULL) {
	fprintf(stderr, "ppak_set_map_buf: can't init copy buffer, abort.\n");
	fflush(stderr);
	return;
    }

    if (p->val.buf != NULL) {
	hpat_cleanup(p->val.buf);
    }


    if(buf == NULL) {
	p->val.buf = NULL;
	return;
    }

    if(memcpy(bufcpy,buf,len) == NULL) {
	fprintf(stderr, "ppak_set_map_buf: can't copy input buffer, abort.\n");
	goto cleanup_and_abort;
    }

    p->val.buf = new_hpat_from_buf(len, bufcpy);
    if(p->val.buf == NULL) {
	fprintf(stderr, "ppak_set_map_buf: can't store buffer, abort.\n");
	goto cleanup_and_abort;
    }

    char *printcpy = malloc(len);
    if(printcpy != NULL) {
	if(memcpy(printcpy,bufcpy,len) != NULL) {
	    if(printcpy[len-2] == '\n') {
		printcpy[len-2] = '\0';
	    }
	}
	fprintf(stderr, "ppak_set_map_buf: %lu, \"%s\"; ", len, printcpy);
	fflush(stderr);
	free(printcpy);
    }else{
	printcpy = hpat_str_ptr(p->val.buf);
	fprintf(stderr, "ppak_set_map_buf: %lu, \"%s\"; ", len, printcpy);
	fflush(stderr);
    };
    return;

cleanup_and_abort:
    fflush(stderr);
    free(bufcpy);

    return;
}

Ppak *ppak_make_ob (int tokid, Hsp *h) {
    if(h == NULL) {
	fprintf(stderr, "ppak_make_ob: Hsp can't be NULL, abort.\n");
	    fflush(stderr);
	return NULL;
    }

    Ppak *p = new_ppak(h->parseroot);
    if(p == NULL) {
	fprintf(stderr, "ppak_make_ob: can't create new ppak, abort.\n");
	    fflush(stderr);
	return NULL;
    }
    p->key.tokid = (Htok_t)tokid;

    fprintf(stderr, "ppak_make_ob( %d, Hsp *h); ", tokid);
    fflush(stderr);

    return p;
}




Mapn *mapnode_add_connection(Mapn *mn, Ppak *ob) {
    if(mn==NULL || ob==NULL) return NULL;

    
    /* get 'new_connection' pointer to new mapnode*/
    /* (use 'mn' as garbage collector) */

    Mapn *new_connection = new_mapnode(ob, mn);
    if(new_connection == NULL) return NULL;

    new_connection->bounds = new_bounds(1);
    if(new_connection->bounds == NULL) goto cleanup_and_abort;


    /* make duplicate bounds info from "ob" to "new connection" */
    /* don't duplicate 'len' information, just axis and offset */
    /* (duplicating the 'len' will create a box boundary for new connection
        which isn't really meaningful, each object we add may have a different
        size box.) */

    if(ob->addr == NULL || ob->addr->bounds == NULL) goto cleanup_and_abort;
    if(ob->addr->bounds->c < 1) goto cleanup_and_abort;
    int i;
    for(i=0; i < ob->addr->bounds->c; i++) {
        bounds_add_axis(new_connection->bounds,
		ob->addr->bounds->a[i].axis,
		ob->addr->bounds->a[i].offset,
		0); 				/* leave 'len' out */
    }


    /* add 'ob' to garbage collection? */


    /* add 'new_connection' to mn's list of connections */
    list_t *newnextcon = mn->con;
    mn->con = new_list_t((void *)new_connection);
    if(mn->con == NULL) {
	fprintf(stderr, "mapnode_add_connection: can't init mapn->con, abort.\n");
	fflush(stderr);
	goto cleanup_and_abort;
    }

    mn->con->next = newnextcon;
    if(newnextcon != NULL)
	newnextcon->prev = mn->con;

    /* increment counter */
    mn->numcon++;

    return new_connection;
cleanup_and_abort:
    mapnode_cleanup(new_connection);
    return NULL;
}

void bounds_grow(bounds_t **dest_p, bounds_t *src) {
    if(dest_p == NULL || src == NULL) {
	fprintf(stderr, "bounds_grow: can't have NULL dest_p or src, abort.\n");
	fflush(stderr);
	return;
    }
    if(src->a == NULL) {
	fprintf(stderr, "src->a NULL, abort.\n");
	fflush(stderr);
	return;
    }

    fprintf(stderr, "bounds_grow: ");
    fflush(stderr);

    if(*dest_p == NULL) { 
	fprintf(stderr, "creating new bounds... ");
	fflush(stderr);
	*dest_p = new_bounds(1);
	if(*dest_p == NULL) {
	    fprintf(stderr, "malloc'd NULL, abort.\n");
	    fflush(stderr);
	    return;
	}
    }
    bounds_t *dest = *dest_p;



    boundl *axis_p;
    int i;
    for(i=0; i < src->c; i++) {
	if((axis_p = bounds_get_axis_ptr(dest, src->a[i].axis)) == NULL) {
	    fprintf(stderr, "adding axis %c... ", src->a[i].axis);
	    bounds_add_axis(dest,
		    src->a[i].axis,
		    src->a[i].offset,
		    src->a[i].len);
	}else{
	    if(axis_p == NULL) {
		fprintf(stderr, "bounds_grow: axis_p is NULL, abort.\n");
		fflush(stderr);
		return;
	    }
	    fprintf(stderr, "updating axis %c... ", axis_p->axis);
	    fflush(stderr);
	    axis_p->offset = (( axis_p->offset < src->a[i].offset )
			      ? axis_p->offset : src->a[i].offset );
	    axis_p->len    = (( axis_p->len    > (src->a[i].len + src->a[i].offset))
			      ? axis_p->len    : (src->a[i].len + src->a[i].offset));
	    fprintf(stderr, "axis %c, offset %lu , len %lu ; ",
		        axis_p->axis, axis_p->offset, axis_p->len);
	    fflush(stderr);
	}

    } 
}

int ppak_add_mapob (Ppak *dst, Ppak *src, int mapid) { /*:= TRUE | ERROR */
    char *mapname;
    if(mapid == 1) mapname = "prop ";
    if(mapid == 2) mapname = "child";
    
    fprintf(stderr, "\n");
    fprintf(stderr, "MAP:%s: ", mapname);
    fprintf(stderr, "ppak_add_mapob( %d, %d ); ", dst->key.tokid, src->key.tokid);
    fflush(stderr);

    if(dst == NULL || src == NULL
	           || src->addr == NULL) {
	fprintf(stderr, "ppak_add_mapob: dst, src or src->addr can't be NULL, abort.\n");
	fflush(stderr);
	return 0;
    }

    /* NOTE: adding a property or a child automatically grows the destination's bounds. */
   
    if(dst->addr == NULL) {
	dst->addr = new_addr_t();
	if(dst->addr == NULL) {
	    fprintf(stderr, "ppak_add_mapob: failed to init dst->addr, abort.\n");
	    return 0;
	}
    }
    bounds_grow(&dst->addr->bounds, src->addr->bounds);

    /* if prop map is NULL, then create mapnode with source */

    Mapn **map;
    if(mapid == 1) map = &dst->prop_root;
    if(mapid == 2) map = &dst->child_root;
    if(mapid != 1 && mapid != 2) {
	fprintf(stderr, "ppak_add_mapob: mapid %d unrecognized, abort.\n", mapid);
	fflush(stderr);
	return -1;
    }

    if(map[0] == NULL) {
	map[0] = new_mapnode(src, NULL);
	if(map[0] == NULL) {
	    fprintf(stderr, "ppak_add_mapob: couldn't init property root, abort.\n");
	    fflush(stderr);
	    return -1;
	}
    }


    /* merge_mapnode_to_tree */
    /* compare dst to src. */

    bounds_t *comparison = NULL;
    comparison = bounds_cmp(src->addr->bounds, map[0]->bounds);
    if(comparison == NULL) {
	fprintf(stderr, "ppak_add_mapob: bounds_cmp fail, abort.\n");
	    fflush(stderr);
	
	return -1;
    }
    /*
       results can be:
       		. same node
		. different node with offset vector */

    /* if same node,
       add mapob from src to dst->mapob */

    if(comparison->pointmatch) {
	mapnode_add_ob(map[0], src);
	bounds_cleanup(comparison);
	return 1;
    }
    bounds_cleanup(comparison);
    comparison = NULL;

    /* if different node,
       follow offset vector, does it intersect one of map->connection?
       		. yes
		    recurse with intersected node
		. no
		    add src to dst->connection */
    int j;
    for(j=0; j < map[0]->numcon; j++) {
	comparison = bounds_cmp(src->addr->bounds, map[0]->connection[j].bounds);
	if(comparison == NULL) {
	    fprintf(stderr, "ppak_add_mapob: bounds_cmp (2) fail, abort.\n");
	    fflush(stderr);
	    return -1;
	}
	if(comparison->pointmatch) {
	    mapnode_add_ob(&map[0]->connection[j], src);
	    bounds_cleanup(comparison);
	    return 1;
	}
	bounds_cleanup(comparison);
	comparison = NULL;
    }
    if(mapnode_add_connection(map[0], src) == NULL) {
	fprintf(stderr, "ppak_add_mapob: can't add connection, skip.\n");
	    fflush(stderr);
	return -1;
    }

    return 1;

    /* PROPERTY MAPPING: */
    /*
       So how this works,
       first we query the property map for a positional node
       which matches src->addr->bounds.
       the property map query returns a positional node
       which either matches or would preceed.

       then we add 'src' to the map at result posnode;
       the subroutine either adds directly to the node if they match
       or it inserts a new posnode which matches directly.
     */



}

int ppak_add_prop_hsp (Ppak *dest, Ppak *src, Hsp *h) {
    int err = ppak_add_mapob(h->src_ref, src, 1);
        err = ppak_add_mapob(dest, src, 1);

    return err;
}



int ppak_add_child (Ppak *dest, Ppak *src) {
    fprintf(stderr, "ppak_add_child( %d, %d ); ", dest->key.tokid, src->key.tokid);
    if(dest == NULL || src == NULL) {
	fprintf(stderr,"ppak_add_child: dest or src can't be NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }
    if(!ppak_add_mapob(dest, src, 2)) {
	fprintf(stderr, "ppak_add_child: trouble forwarding to ppak_add_mapob, abort.\n");
	    fflush(stderr);
	return -1;
    }
    return 1;
}

int ppak_add_child_hsp (Ppak *dest, Ppak *src, Hsp *h) {
    int err = 0;
        err = ppak_add_mapob(h->src_ref, src, 2); /* 2 == child map */
        err = ppak_add_mapob(dest, src, 1);	  /* 1 == prop map  */

    return err;
}

int ppak_list_map (Ppak *src, int mapid) {
    if(mapid != 1 && mapid != 2) {
	fprintf(stderr, "ppak_list_map: mapid %d unrecognized, abort.\n", mapid);
	fflush(stderr);
	return -1;
    }
    if(src == NULL) {
	fprintf(stderr, "ppak_list_map: src NULL, abort.\n");
	fflush(stderr);
	return -1;
    }


    char *mapname;
    Mapn **map;
    if(mapid == 1) { map = &src->prop_root;  mapname = "prop "; }
    if(mapid == 2) { map = &src->child_root; mapname = "child"; }

    printf("ppak_list_map: %s\n", mapname);

    Ppak *curob;
    Htok_t curtokid;
    char *curval;
    list_t *lt;

    Mapn *curmapn = map[0];

    while(curmapn != NULL) {
	if(curmapn->ob != NULL) {
	    if(curmapn->ob->ob != NULL) {
		lt = curmapn->ob;
		while(lt != NULL) {

		    curob = lt->ob;
		    curtokid = curob->key.tokid;
		    if(curob->val.buf != NULL) {
			curval = hpat_str_ptr(curob->val.buf);
		    }else{
			curval = "(empty value)";
		    }
		    printf("%p:%p:  %d  %s\n", lt, curob, curtokid, curval); 

		    lt = lt->next;
		}
	    }
	}
	curmapn = curmapn->garbage;
    }
    return 0;


}

/**************** PPAK REFRESH ********************/


void ppak_kill_all_frags( Dpak *p ) {
}
#define IN_SYNC 1
void ppak_set_storage_updated ( Dpak *p, int flag ) {
    if(p == NULL) return;
    p->storage_updated = flag;
}

int ppak_refresh ( Ppak *p, Axpa *a, Hsp *h ) { /* == SUCCESS || ERROR */
    /* version 2 hiena - fetch stream and filter into fragments */
    if( p == NULL || a == NULL || h == NULL ) return -1;


    scannerserver *filter = NULL;
    dpakserver    *server = NULL;
    addr_t        *addr   = NULL;
    dpakstream    *fp     = NULL;



    /* version 2 hiena - I think these have to be bison/flex generated */
    if(( filter = ppak_get_filter(p) ) == NULL) /*ignore*/ ;
    if(( server = ppak_get_server(p) ) == NULL) return -2;
         addr   = ppak_get_addr(p);

    if( server->open == NULL ) {
	fprintf(stderr, "ppak_refresh: server->open NULL, abort.\n");
	fflush(stderr);
	return -3;
    }
         fp     = server->open(addr, "r", server);

    /* version 2 hiena - if storage gets updated, it pulls a rug out from under the entire house of cards. */
    ppak_kill_all_frags(p);
    /* -- */

    /* DO FILTER HERE */
    /* fills p's frags with data (or at lease addresses ) */
    /* if filter NONE, default filter. */
    do_filter(filter, p);

    /* PARSE DPAK */
    do_scanners(p, a->scanners );
   
    server->close(fp);

    ppak_set_storage_updated(p, IN_SYNC );

    return 1;	/* TBD error fielding */


}

/* ---------  DPAK SET VALUE --------*/
/* these use dpakstream functions */



/**************************** DPAK ACCESSOR VAL *********************************/

int dpak_set_stream_cell ( Dpak *p, dscell_t *c ) {
    if(p == NULL) return -1;
    /* this will forget the previous values and may leak memory if you don't cleanup elsewhere */
    d->val.first_cell = c;
    d->val.last_cell  = c;
    return 0;
}

static void ppak_val_free_if_set ( Ppak *pp ) {
    /* TODO: update to work with fragmap */

    if(pp->val.buf != NULL)
	hpat_cleanup(pp->val.buf);
    pp->val.first_frag = NULL;
    pp->val.last_frag = NULL;
}

int ppak_verify_val(Ppak *p) { /*:= TRUE | FALSE */
    if(p == NULL) return 0;

    if(p->val.is_parent_backed)
    {
	/* value is backed by parent's storage */
	if(p->val.buf == NULL)
	/* val.buf == NULL is OK as long as parent and location data are good */
	{
	    if(p->val.location.parent == NULL)
	    {
		/* FAIL */
		return 0;
	    }
	    if(p->val.location.len == 0)
	    {
		/* FAIL */
		return 0;
	    }
	    return 1;
	}
	/* if val.is_parent_backed AND val.buf != NULL
	   then we expect val.buf to be a copy of info from parent */ 
	
	/* TBD:  SYNC BETWEEN val.buf and val.location */
	printf("ppak_verify_val: val.buf and val.is_parent_backed, ambiguous.\n");
	    fflush(stderr);
	return 1;
    }else{
	/* value is backed by other storage */
	if(p->val.buf == NULL)
	/* val.buf == NULL is OK as long as p->addr->pat and p->srv aren't NULL. */
	{
	    if((p->addr != NULL && p->addr->pat == NULL) || p->server == NULL)
	    {
		/* FAIL */
		return 0;
	    }
	    return 1;
	}else{
	    /* if val.buf != NULL AND p->addr->pat and p->server aren't NULL
	       we expect val.buf to be a product of the other two. */

	    if((p->addr != NULL && p->addr->pat != NULL) && p->server != NULL)
	    {
		/* TBD: SYNC BETWEEN val.buf and server,addr */
		printf("ppak_verify_val: val.buf and server and addr, must sync.\n");
	    fflush(stderr);
		return 1;
	    }else{
	    /* if val.buf != NULL AND p->addr->pat OR p->server == NULL
	       we expect val.buf to be original un-backed data. */
		return 1;
	    }
	}
    }
}


/**** AS STRING ****/
/* TBD: need to update this to use a 'fragmap' as value container. */

/* rewrite this function to use dsopen, dswrite */
/* requires a server be assigned to the dpak prior. */

int ppak_set_val_str(Ppak *p, const char *str) { /* == TRUE | FALSE */

    /* FAST, UGLY, POSSIBLY MISBEHAVING VERSION 2 */
    
    /* receives a string pointer
       opens a dpakstream on "p"
       writes the contents of the string to dpakstream
       closes dpakstream */

    if (p == NULL || str == NULL) return 0;

    
    if (ppak_get_server( p ) == NULL) {
	fprintf(stderr, "hiena: ppak_set_val_str: requires dpak's server to be set, abort.\n");
	return 0;
    }

    if (ppak_get_filter( p ) == NULL) {
	fprintf(stderr, "hiena: ppak_set_val_str: requires dpak's filter to be set, abort.\n");
	return 0;
    }

    size_t strsiz = ((strlen(str)+1)*char);

    if (strsiz != 0) {
        dpakstream *ds = dpak_dsopen( p, "w+" );	/* write-truncate plus read */
	if (ds == NULL) {
	    switch ppak_get_error( p )
	    {
		case DS_WRITELOCK:
		    fprintf(stderr, "hiena: ppak_set_val_str: dpak_dsopen failed, DS_WRITELOCK.\n");
		    break;
	    }
	    return 0;
	}
	dpak_dswrite( str, strsiz, 1, ds ); 
	dpak_dsclose( ds );
    }else{
	return 0;
    }
    return 1;
}




char *ppak_get_str( Ppak *p ) {
    /* MALLOC ALERT: creates duplicate string */
    /* result must be free'd */
    if(p == NULL) return NULL;

    char *dst = NULL;


    if( p->val.is_parent_backed == 1 ) {
	/*TBD*/
    }
    if( p->val.first_frag != NULL ){
	bounds_t *b = frag_get_bounds(p->val.first_frag);
	size_t n = (size_t)bounds_get_len( b, 'x' );

	dst = strndup( (char *)frag_get_buf(p->val.first_frag), n );
	return dst;
    }
    return NULL;
}


/* STRING VECTORS / ARRAY VALUE */
int ppak_set_strv(Ppak *pp, int argc, char *argv[]){
    ppak_val_free_if_set(pp);
    if((pp->val.buf = new_hpat_from_argv(argc, argv))!=NULL)
        return 0;
    return -1;
}



/* BUFFER VAL */

int
ppak_set_buf(Ppak *pp, Hpat *buf) {
    /* buf NULL is ok */
    /* pp  NULL is NOT ok */
    if(pp==NULL){
	fprintf(stderr,"libhiena:ppak_set_buf fail on NULL target.\n");
	    fflush(stderr);
	return -1;
    }
    pp->val.buf = buf;
    return 0;
}


