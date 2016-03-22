/* _HIENA_SERVERLIB_C_ */

/** \file serverlib.c */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../progconf/paths.h"
#include <gob.h>
#include <hiena/server_module.h>
#include <libgen.h>
#include "server_callbacks.c"
#include "serverlib.h"

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_LTDL_H
# include <ltdl.h>
#endif

static int print_module_info ( const char * filename, void * data ) {
    if( filename == NULL ) return -1;
    printf("%s\n", filename);
    return 0;
}



/************************************************************/
/*            Serverlib Entry Object Methods                */
/************************************************************/


void serverlibentry_cleanup ( void * ptr ) {
    if( ptr == NULL ) return;

    struct hiena_serverlib_entry * e = ptr;

    if( e->ops != NULL )
	free( e->ops );

#if HAVE_LTDL_H
    if( e->handle != NULL )
	lt_dlclose( e->handle );
#endif

    if( e->filepath != NULL) {
	free( e->filepath );
    }
    if( e->name != NULL ) {
	free( e->name );
    }

    free( e );
}






/**
 * This function's return type and args are specified by ltdl.h
 * . We pass a pointer to this function to the lt_ldforeachfile().
 */

static int serverlibentry_init ( const char * filename, void * data ) {
    if( filename == NULL || data == NULL ) return -1;
    struct hiena_serverlib * serverlib = data;


#if HAVE_LTDL_H
    /* Create new serverlib entry */
    struct hiena_serverlib_entry * e = malloc(sizeof(*e));
    memset(e, 0, sizeof(*e));
    gob_set_cleanup( e, serverlibentry_cleanup );
    e->filepath = strdup(filename);
    e->name = strdup(basename(filename));
    printf("e->filepath == %s\n", filename );

    /* Load module into entry */
    lt_dlhandle module = lt_dlopenext( filename );
    if( module != NULL ) {
	e->handle = module;
    }else{
	printf("e->handle == NULL\n");
    }

    /* Create new serverops structure */
    struct hiena_serverops * op = malloc(sizeof(*op));
    memset(op, 0, sizeof(*op));
    e->ops = op;

    /* load symbols into serverops */
    op->stat = lt_dlsym( module, "server_stat" );
    op->open = lt_dlsym( module, "server_open" );
    op->close = lt_dlsym( module, "server_close" );
    op->read  = lt_dlsym( module, "server_read" );
    op->write = lt_dlsym( module, "server_write" );


    if( op->stat == NULL ) printf( "op->stat == NULL\n");
    if( op->open == NULL ) printf( "op->open == NULL\n");
    if( op->close == NULL) printf( "op->close == NULL\n");
    if( op->read == NULL ) printf( "op->read == NULL\n");
    if( op->write == NULL) printf( "op->write == NULL\n");


    /* add entry to serverlib */
    if( serverlib->firstentry == NULL ) {
	serverlib->firstentry = e;
	serverlib->lastentry = e;
    } else {
	gob_add_nextgarb( serverlib->lastentry, e );
	serverlib->lastentry = e;
    }


#endif

    return 0;
}


/** 
 * Initialize the server library
 *
 */
void * serverlib_init ( size_t len, void * serverlibpathsrc ) {
    if( len == 0 || serverlibpathsrc == NULL ) return NULL;

    /** create library object */
    struct hiena_serverlib * serverlib = malloc(sizeof(*serverlib));
    memset(serverlib, 0, sizeof(*serverlib));

    /** Setup callbacks. */
    struct hiena_server_callbacks * cb = malloc(sizeof(*cb));
    memset(cb, 0, sizeof(*cb));
    serverlib->callbacks  = cb;

    cb->dir_new           = dir_new;
    cb->dir_addr_set      = dir_addr_set;
    cb->dir_map_add       = dir_map_add;
    cb->dir_add           = dir_add;
    cb->map_new_freeagent = map_new_freeagent;

    /** Load library */
    char * serverlibpath  = strndup( (char *)serverlibpathsrc, len );


    
#if HAVE_LTDL_H
    int err;
    /* LTDL_SET_PRELOADED_SYMBOLS(); */

    serverlib->ltdl = lt_dlinit();
    if( serverlib->ltdl == 0 )
    {
	err = lt_dlforeachfile( serverlibpath, serverlibentry_init, &(serverlib->firstentry) );
    }
#endif
    free( serverlibpath );

    return serverlib;
}






/**
 * Cleans up the server library.
 */
void serverlib_cleanup ( void * ptr ) {
    if( ptr == NULL ) return;

    struct hiena_serverlib * serverlib = ptr;

    if( serverlib->firstentry != NULL )
	gob_cleanup( serverlib->firstentry );

#if HAVE_LTDL_H
    if( serverlib->ltdl == 0 )
    {
	lt_dlexit();
    }
#endif

    if( serverlib->callbacks != NULL )
	free( serverlib->callbacks );
    free( serverlib );
    return;
}


/**
 * Retrieve server operations structure from named server.
 * First match wins.
 */
struct hiena_serverops * serverlib_serverops ( void * serverlibptr, size_t len, void * nameptr ) {
    if(serverlibptr == NULL || len == 0 || nameptr == NULL) return NULL;

    struct hiena_serverlib * serverlib = serverlibptr;
    struct hiena_serverlib_entry * cur = serverlib->firstentry;

    while( cur != NULL ) {
	if( cur->name != NULL && strncmp(cur->name, nameptr, len) == 0 ) {
	    if( cur->ops != NULL ) {
		return cur->ops;
	    }
	}
    }
    return NULL;
}

/*end_HIENA_SERVERLIB_C_*/
