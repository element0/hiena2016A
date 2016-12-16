#include "list_t.h"

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
