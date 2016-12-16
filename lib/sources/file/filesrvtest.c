#include "filesrv.c"
#include "../../../src/server_callbacks.c"


int main( int argc, char * argv[] ) {
    if( argc != 2 ) {
	printf("usage: %s filepath\n", argv[0]);
	return -1;
    }

    struct hiena_server_callbacks *h = malloc( sizeof(struct hiena_server_callbacks));
    memset( h, 0, sizeof(struct hiena_server_callbacks) );

    h->dir_new      = dir_new;
    h->dir_addr_set = dir_addr_set;
    h->dir_map_add  = dir_map_add;
    h->dir_add      = dir_add;
    h->map_new_freeagent = map_new_freeagent;

    void *object;
    void *ptr;
    size_t rread;

    object = server_open( (void *)argv[1], strlen(argv[1]), "r" );
    if ( object != NULL ) {
	rread = server_read( &ptr, 10, object, h );
	if ( rread > 0 ) {
	    printf( "FILE: read %ld bytes.\n", rread );
	    fwrite( ptr, 1, rread, stdout );
	    printf( "\n" );
	    fflush( stdout );
	    ;
	}
	//if(ptr != NULL)
	  //  free(ptr);
	server_close( object );
    }


    /* test EBADF */
    /*
    object = server_open( "/home/raygan/test/dboy/cosmostest/gobbledeegook", "r" );

    if ( object != NULL ) {
	rread = server_read( &ptr, 1, object, h );
	if ( rread > 0 ) {
	    ;
	}
	server_close( object );
    }
    */

    free( h );

    return 0;
}
