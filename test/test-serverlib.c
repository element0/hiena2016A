/* USE THIS IF NOT USING AUTOCONF */
#define HAVE_LTDL_H 1

#include <stdio.h>
#include "../progconf/paths.h"
#include <gob.c>
#include "../src/sourcelib.c"


int main ( int argc, char *argv[] ) {
    if( argc != 2 ) {
	printf("usage: %s filepath\n", argv[0]);
	return -1;
    }

    struct hiena_serverlib * serverlib =
	serverlib_init( strlen(HIENA_SERVERLIB_PATH), HIENA_SERVERLIB_PATH );

    struct hiena_serverops * ops = serverlib_serverops( serverlib, strlen("file"), "file" );

    if( ops->open != NULL &&
	ops->close != NULL &&
	ops->read != NULL)
    {
	void * fh = ops->open( (void *)argv[1], "r" );
	if( fh != NULL ) {
	    printf( "opened\n" );
	    size_t size = 10;
	    void * data = malloc(sizeof(char)*size);
	    size_t numread = ops->read( data, size, fh, serverlib->callbacks );
	    printf( "read %d\n", numread );

	    size_t numwrit = fwrite( data, numread, 1, stdout );
	    printf( "write %d\n", numwrit );

	    free(data);

	    if( ops->close( fh ) == 0 ) {
		printf( "closed\n");
	    }else{
		printf( "error on close\n");
	    }
	}
    }else{
	printf("ops->open|close|read == NULL\n");
    }

    serverlib_cleanup(serverlib);

    return 0;
}
