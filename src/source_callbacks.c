/** @file server_callbacks.c */

#include "domainhandle.h"

/* Returns child or NULL */
void * dir_new_child ( void * d_name, size_t d_namelen, void * addr, size_t addrlen, char * serversign ) {
    void * obindex = NULL;
    void * c = objectindex_new_object ( obindex, d_name, d_namelen );
    return NULL;
}

/* GENERIC SERVER MODULE FUNCTIONS */

int    hiena_stat (void *, struct stat *) {
    return 0;
}

struct hiena_server_file_handle *
hiena_open (addr_t * addr, size_t unused, const char * mode, struct hiena_server_callbacks * h) {
    /* h can be null. addr->serverlib->cb contains callbacks. */
    if( addr == NULL || mode == NULL ) return NULL;

    return NULL;
}
int    hiena_close (void *, struct hiena_server_callbacks *) {
    return 0;
}

size_t hiena_read  (void **, size_t, void *, struct hiena_server_callbacks *) {
    return 0;
}
size_t hiena_write (void *,  size_t, void *, struct hiena_server_callbacks *) {
    return 0;
}
int    hiena_seek  (void * fh, off_t offset, int whence) {
    return 0;
}

/* end file server_callbacks.c */
