/* SERVER MODULE: localhost filesystem

architecture: linux


   */
#include "../../../include/hiena/server_module.h"	/* generic server header for all hiena server modules */
#include <sys/stat.h>	/* stat() */
#include <unistd.h>	/* stat() */
#include <stdio.h>	/* FILE */
#include <stdlib.h>	/* malloc */
#include <string.h>	/* memset */

struct server_localmem_own {
    const char *addr;
    size_t size;
    FILE *fp;
    struct stat sb;
};

static struct server_localmem_own * new_own() {
    struct server_localmem_own *own = malloc(sizeof(*own));
    memset(own, 0, sizeof(*own));
    return own;
}

void destroy_own( struct server_localmem_own *own ) {
    if( own == NULL )
	return;
    free(own);
}


void * server_open ( void *addr, size_t size, const char *mode ) {
    if ( addr == NULL || mode == NULL )
	return NULL;

    struct server_localmem_own *own = new_own();
    own->addr = addr;
    own->size = size;
    
    FILE *fp = fmemopen( addr, size, mode );
    own->fp   = fp;

    return (void *)own;
}

size_t server_read ( void * ptr, size_t size, void * object, struct hiena_source_callbacks * h ) {
    if( ptr == NULL || size == 0 || object == NULL || h == NULL )
	return 0;

    struct server_localmem_own *own = object;

    if( own == NULL )
	return 0;

    if ( own->fp != NULL ) {
	size_t rread = fread( ptr, 1, size, own->fp );
	return rread;
    }

    return 0;
}

int server_close ( void *object ) {
    if ( object == NULL )
	return -1;
    
    struct server_localmem_own *own = object;

    if ( own->fp != NULL ) {
	int err = fclose( (FILE *)own->fp );
	if ( err == EOF ) {
	    if ( errno == EBADF ) {
		;
	    }
	    return -1;
	}
    }
    destroy_own(own);
    return 0;
}
