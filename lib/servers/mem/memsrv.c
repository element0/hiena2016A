/* SERVER MODULE: localhost memory

architecture: linux

*/

#include "../server.h" /* generic server header for all hiena server modules */

struct server_localmem_own {
    void *addr;
};

void *server_open ( void *addr, const char *mode, struct hiena_transaction *h ) {
    return NULL;
}

size_t server_read ( void **ptr, size_t size, void *object, struct hiena_transaction *h ) {
    return 0;
}

int server_close ( void *object, struct hiena_transaction *h ) {
    if ( object == NULL || h == NULL ) return -1;

    return 0;
}
