#include "addr_t.h"


/*== OBJECT: addr_t ==*/

addr_t *new_addr_t () {
    addr_t *a = malloc(sizeof(*a));
    if(a != NULL)
	memset(a, 0, sizeof(*a));
    return a;
}

void addr_t_cleanup ( addr_t *a ) {
    if(a == NULL) return;
    if(a->ptr != NULL)
	free(a->ptr);
    hpat_cleanup(a->pat);
    bounds_cleanup(a->bounds);
    
    free(a);
}
/*--------*/
