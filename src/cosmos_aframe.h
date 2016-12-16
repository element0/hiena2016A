#ifndef _COSMOS_AFRAME_H_
#define _COSMOS_AFRAME_H_

/** @file
 *
 * Domain cell API.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fuse_lowlevel.h>


/**
 * A cosmos aframe performs the function of a path segment.
 *
 * "A-frame" stands for "access frame".
 *
 *
 * A cosmos-hiena file system has two kinds of inodes: dcels and aframes.
 * The dcel records the data structure of boundeded stream of data.
 * The aframe records context information used to access the dcel from
 * a given access path.  Different paths to the same location have different
 * contexts.  The dcel may contain map cells created by one aframe but
 * not another.  Map cels may have user restrictions.  But even if a map cell
 * has been produced from one context, it may be fair for the other context
 * to access it.  But this doesn't mean the other context inherits any of the
 * other.
 *
 * The aframe structure functions as the primary inode exposed to the
 * FUSE interface.  The dcel is handled internally.
 *
 */
struct cosmos_aframe {
	struct stat *statbuf;
}


/**
 * Get stat buffer for aframe
 *
 * This follows the format for stat (2).
 */
int aframe_stat( struct cosmos_aframe *aframe, struct stat *buf );


/**
 * Re-entrant read directory function.
 *
 * Follows the format of readdir_r.
 * Returns a conventional dirent.
 * Proof of concept only uses d_ino and d_name per POSIX.
 *
 * This version does not rely on calling aframe_opendir but
 * instead uses struc cosmos_aframe as a DIR object.
 */
int aframe_readdir_r( struct cosmos_aframe *aframe, struct dirent *entry, struct dirent **result );

/**
 * Open a file stream on a aframe.
 *
 * Proof of concept uses return type void*.
 *
 * 'mode' follows FUSE requirement rather than fopen (3).
 */
void *aframe_fopen( struct cosmos_aframe *aframe, int mode );

/**
 * Read a file stream opened from a aframe.
 *
 * Proof of concept uses file handle 'fh' type void*.
 *
 */
size_t aframe_fread( char *buf, size_t size, void *fh );

/**
 * Close file stream handle.
 *
 * Proof of concept types fh to void *.
 *
 */
int aframe_fclose( void *fh );

#endif /*!_COSMOS_AFRAME_H_*/
