/* HEINA_MAPPING_C */
#include "domainhandle.h"

/* USED BY "server_callbacks.c", scanners, lookup, sql */
/* TODO: headerize me. */


/* FRAGBUF */

void * fragbuf_init ( void * ptr, size_t len ) {
    return NULL;
}

/* MAP */

void *map_new_freeagent( void *buf, size_t len, const char *pname ) {
    char * str = strndup(buf, len);
    printf("map_new_freeagent: %s\n", str);
    free(str);
    return NULL;
}


/* OBJECT INDEX */

void * objectindex_new_object ( void * ob_index, void * ptr, size_t len ) {
    void * fb = fragbuf_init( ptr, len );
    return NULL;
}



