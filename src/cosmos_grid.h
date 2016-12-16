#ifndef _COSMOS_GRID_H_
#define _COSMOS_GRID_H_

/** @file
 *
 * Cosmos grid
 *
 */

#include "hiena_dcel.h"
#include "cosmos_aframe.h"
#include <fuse_lowlevel.h>

struct cosmos_grid {
	void *placeholder;
};

/**
 * Returns a channel to a grid host.
 *
 * If there is already a running gridhost process,
 * this merely returns a channel to the running host.
 *
 * 'griduser' can be NULL.  If griduser is not null
 * it must be an object prepared with griduser_int().
 */
void *grid_init( void *griduser );

/**
 * Add a URL to the URL database
 *
 * If 'url2' is not NULL, binds URL to bundle identified by 'url2'.
 * 'url2' may indentify a bundle of several other urls registered with host.
 * It is OK for 'url2' to be NULL.
 */
int grid_add_url( void *grid, const char *url, const char *url2 );

/**
 * Helper function creates 'file' scheme URL's from pathnames.
 */
const char *grid_filepath_to_URL( const char *filepath );

/**
 * Lookup an access frame in the grid based on a parent
 * and a lookup string.
 *
 * This function will use the 'lookup' module provided by the current
 * context frame.
 */
struct cosmos_aframe *grid_lookup( void *grid, fuse_ino_t par,
		                         const char *name );

/**
 * Gets a context frame from an ino.
 */
struct cosmos_aframe *grid_get_inode( void *grid, fuse_ino_t ino );


#endif /*!_COSMOS_GRID_H_*/
