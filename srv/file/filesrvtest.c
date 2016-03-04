#include "filesrv.c"


int my_new_child( int id ) {
    static int child_id = 0;
    return child_id++;
}

int my_child_set_prop( const char *p_name, void *p_val, int child_id ) {
    if( p_name == NULL || p_val == NULL ) return -1;
    printf( "my_child_set_prop( \"%s\", \"%s\", %d );\n", p_name, (char *)p_val, child_id );

    return 0;
}

int my_child_set_addr( void *addr, int child_id ) {
    if( addr == NULL ) return -1;
    printf( "my_child_set_addr( \"%s\", %d );\n", (char *)addr, child_id );
    return 0;
}

int main() {
    void *ptr;
    struct hiena_transaction *h = malloc(sizeof(*h));
    memset(h,0,sizeof(*h));

    h->new_child      = my_new_child;
    h->child_set_prop = my_child_set_prop;
    h->child_set_addr = my_child_set_addr;

    void *object;
    size_t rread;

    /* test dir type */
    object = server_open( "/home/raygan/test/dboy/cosmostest", "r", h );
    if ( object != NULL ) {
	rread = server_read( &ptr, 1, object, h );
	if ( rread > 0 ) {
	    ;
	}
	server_close( object, h );
    }

    /* test file type */
    object = server_open( "/home/raygan/test/dboy/cosmostest/file.txt", "r", h );
    if ( object != NULL ) {
	rread = server_read( &ptr, 10, object, h );
	if ( rread > 0 ) {
	    printf( "FILE: read %ld bytes.\n", rread );
	    fwrite( ptr, 1, rread, stdout );
	    printf( "\n" );
	    fflush( stdout );
	    ;
	}
	free( ptr );
	server_close( object, h );
    }

    /* test EBADF */
    object = server_open( "/home/raygan/test/dboy/cosmostest/gobbledeegook", "r", h );

    if ( object != NULL ) {
	rread = server_read( &ptr, 1, object, h );
	if ( rread > 0 ) {
	    ;
	}
	server_close( object, h );
    }

    free( h );

    return 0;
}
