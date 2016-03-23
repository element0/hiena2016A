#include <stdlib.h>
#include <string.h>

struct hiena_domainstream {
    void * ops;
    char * serversign;
    void * addr;
};

struct hiena_domainstream * domainstream_new () {
    struct hiena_domainstream * ds = malloc( sizeof( *ds ));
    memset( ds, 0, sizeof( *ds ) );
    return ds;
}

void domainstream_cleanup ( struct hiena_domainstream * ds ) {
    if( ds == NULL ) return;
    free( ds );
}
