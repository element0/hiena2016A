/* _HIENA_SERVERLIB_H_ */
#ifndef _HIENA_SERVERLIB_H_
#define _HIENA_SERVERLIB_H_

/** \file serverlib.h
 *
 * Interface for the hiena server library.  One library per host.
 * Library is loaded by serverlib_init(SEARCHPATH).
 *
 * Access methods are provided for fetching server ops for each server module
 * and for fetching the callback structure to supply to the ops.
 *
 * The server entries themselves are intended to be opaque to hiena.
 * And the implementation of each hosts server library is flexible.
 */

#include <sys/types.h>	/* off_t, size_t */
#include <gob.h>

/**
 * A server module after it's been loaded from filepath.
 */
struct hiena_serverlib_entry {
    gob  gobheader;	/**< SPACE for Generic OBject header */
    char * name;	/**< The short name of the dylib without path or extension */
    char * filepath;	/**< Where the dylib was loaded from */
    void * handle;	/**< The handle would be a lt_dlhandle in the case of libtool ltdl */
    			/**< We just don't require libtool implementation. */
    void * ops;		/**< The symbols from the server module */
};

/**
 * Main server library object.
 */
struct hiena_serverlib {
    struct hiena_serverlib_entry  * firstentry;
    struct hiena_serverlib_entry  * lastentry;
    struct hiena_server_callbacks * callbacks;
    int ltdl;	/**< for use with ltdl.h */
};


void * serverlib_init( size_t len, void * serverlibpath );
void   serverlib_cleanup( void * serverlib );
struct hiena_serverops *        serverlib_serverops( void * serverlib, size_t len, void * name );
struct hiena_server_callbacks * serverlib_callbacks( void * serverlib, size_t len, void * name );
int    serverlib_autoselect( void * serverlib, size_t len, void * addr,
	                                       size_t * retlen, void ** retname );


#endif /*!_HIENA_SERVERLIB_H_*/
