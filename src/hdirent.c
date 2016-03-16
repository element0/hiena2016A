/* HIENA_DIRECTORY_ENTRY_C */

#include "hdirent.h"
#include "string.h"

hdirent_t * new_hdirent() {
    hdirent_t *e = malloc(sizeof(*e));
    if(e != NULL) memset(e, 0, sizeof(*e));
    return e;
}

void hdirent_cleanup ( hdirent_t *e ) {
    if(e == NULL) return;
    if(e->name_str != NULL) free(e->name_str);
    if(e->addr_buf != NULL) free(e->addr_buf);
    free(e);
    return;
}
