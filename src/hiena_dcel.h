#ifndef _HIENA_DCEL_H_
#define _HIENA_DCEL_H_

/** @file
 *
 * Domain cell API.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fuse_lowlevel.h>


/**
 * the basic data type for hiena.
 *
 * the dcel structure functions as an inode for the filesystem.
 */
struct hiena_dcel {
	struct stat *statbuf;
}


/**
 * Get stat buffer for dcel
 *
 * This follows the format for stat (2).
 */
int dcel_stat( struct hiena_dcel *dcel, struct stat *buf );


/**
 * Re-entrant read directory function.
 *
 * Follows the format of readdir_r.
 * Returns a conventional dirent.
 * Proof of concept only uses d_ino and d_name per POSIX.
 *
 * This version does not rely on calling dcel_opendir but
 * instead uses struc hiena_dcel as a DIR object.
 */
int dcel_readdir_r( struct hiena_dcel *dcel, struct dirent *entry, struct dirent **result );

/**
 * Open a file stream on a dcel.
 *
 * Proof of concept uses return type void*.
 *
 * 'mode' follows FUSE requirement rather than fopen (3).
 */
void *dcel_fopen( struct hiena_dcel *dcel, int mode );

/**
 * Read a file stream opened from a dcel.
 *
 * Proof of concept uses file handle 'fh' type void*.
 *
 */
size_t dcel_fread( char *buf, size_t size, void *fh );

/**
 * Close file stream handle.
 *
 * Proof of concept types fh to void *.
 *
 */
int dcel_fclose( void *fh );

#endif /*!_HIENA_DCEL_H_*/
