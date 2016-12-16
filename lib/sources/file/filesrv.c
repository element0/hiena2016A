/* SERVER MODULE: localhost filesystem

   */
#include "../../../include/hiena/source_module.h"	/* generic server header for all hiena source modules */
#include "../../../include/hiena/scanner_module.h"
#include <dirent.h>	/* DIR */
#include <sys/stat.h>	/* stat() */
#include <unistd.h>	/* stat() */
#include <stdio.h>	/* FILE */
#include <stdlib.h>	/* malloc */
#include <string.h>	/* memset */

struct server_localfile_own {
    const char *addr;
    FILE *fp;
    DIR  *dirp;
    struct stat sb;
};

static struct server_localfile_own *new_own() {
    struct server_localfile_own *own = malloc(sizeof(*own));
    memset(own, 0, sizeof(*own));
    return own;
}

static void destroy_own( struct server_localfile_own *own ) {
    if( own == NULL )
	return;
    free(own);
}

/** for compatibility with hiena/server_module.h 'size' must be included,
 *  but in this server implementation it is ignored.
 */
void *server_open ( void *addr, size_t size, const char *mode, struct hiena_source_callbacks * h ) {
    if ( addr == NULL || mode == NULL )
	return NULL;

    struct server_localfile_own *own = new_own();
    if( own == NULL ) return NULL;
    own->addr = addr;

    /* test for type, file or dir */
    int err = stat( (const char *)addr, &own->sb );
    if ( err == -1 ) {
	switch ( errno ) {
	    case EACCES:
		printf( "Search perm denied for one of the directories in pathname.\n");
		break;
            case EBADF:
		printf( "File descriptor is bad.\n");
		break;
	    case EFAULT:
		printf( "Bad address.\n");
		break;
	    case ELOOP:
		printf( "Too many symbolic links encountered.\n");
		break;
	    case ENAMETOOLONG:
		printf( "Pathname is too long.\n");
		break;
	    case ENOENT:
		printf( "A component of pathname does not exist -- or pathname is empty string.\n");
		break;
	    case ENOMEM:
		printf( "Out of kernel memory.\n");
		break;
	    case ENOTDIR:
		printf( "A component of the path prefix of pathname is not a directory.\n" );

		break;
	    case EOVERFLOW:
		printf( "Pathname or fd refers to a file whose size, inode number, or numbe rof blocks cannot be represented in, respectively, the types off_t, ino_t or blkcnt_t.  This error can occur when, for example, an application compiled on a 32-bit platform without -D_FILE_OFFSET_BITS=64 calls stat() on a file whose size exceeds (1<<31)-1 bytes.\n");
		break;

	}
	destroy_own(own);
	return NULL;

    }

    /* if dir type */
    if (S_ISDIR(own->sb.st_mode)) {

	DIR *dirp = opendir( (const char *)addr );
	if ( dirp == NULL ) {
	    switch ( errno ) {
		case EACCES:
		    ;
		case EBADF:
		    ;
		case EMFILE:
		    ;
		case ENFILE:
		    ;
		case ENOENT:
		    ;
		case ENOMEM:
		    ;
		case ENOTDIR:
		    ;
		default:
		    ;
	    }
	}

	own->dirp = dirp;
	own->fp   = NULL;

	return (void *)own;
    }
    
    /* if file type */
    if ( S_ISREG(own->sb.st_mode) ) {
	FILE *fp = fopen( (const char *)addr, mode );
	own->fp   = fp;
	own->dirp = NULL;

	return (void *)own;
    }

    return NULL;
}

size_t server_read ( void * ptr, size_t size, void * object, struct hiena_source_callbacks * h ) {
    if( ptr == NULL || size == 0 || object == NULL || h == NULL )
	return 0;

    struct server_localfile_own *own = object;

    if( own == NULL )
	return 0;

    /* read dir */
    if ( own->dirp != NULL && own->fp == NULL ) {

	void  * m;
	void  * c;
	void  * d;
	struct dirent *dp;
	char  * d_name_var;
	char  * child_addr;
	size_t d_name_len;
	size_t child_addr_len;

	/** create hiena directory object */
	d = h->dir_new();

	/** read each directory entry from server,
	 *  create children of 'd'.
	 */
	while (( dp = readdir( own->dirp )) != NULL ) {

	    /** get child's basename 'd_name_var'
	     */

	    d_name_len 	= strlen(dp->d_name);
	    d_name_var	= strndup(dp->d_name, d_name_len+1);
	    
	    /** create child's filepath 'child_addr'
	     */

	    child_addr_len = strlen( own->addr ) + 1 + d_name_len + 1;
	    child_addr     = malloc( child_addr_len * sizeof(char) );
	    snprintf( child_addr, child_addr_len, "%s/%s", own->addr, d_name_var );
	    child_addr[ child_addr_len - 1 ] = '\0';


	    /** create a new attribute map for child
	     *  with its basename as the map value.
	     */

	    //
	    // NEWAPI!   dir_new_child ( d_name, d_namelen, addr, addrlen, child_serversign )
	    //

	    m = h->map_new_freeagent( d_name_var, d_name_len, "name" ); 
	    free(d_name_var);

	    /** create new child object
	     */
	    c = h->dir_new_child ( d_name_var, d_name_len, child_addr, child_addr_len, "file" );


	    /** add the attribute map to the child
	     */
	    h->dir_map_add(c, m);

	    /** set child's domain source address and server module name
	     */
	    h->dir_addr_set(c, child_addr, "file" );

	    /** add child to directory 'd'
	     */
	    h->dir_add(d, c);

	    free(child_addr);
	}
    }

    /* read file */
    if ( own->fp != NULL && own->dirp == NULL ) {
	size_t rread = fread( ptr, 1, size, own->fp );
	return rread;
    }

    return 0;
}

int server_close ( void *object, struct hiena_source_callbacks * h ) {
    if ( object == NULL )
	return -1;
    
    struct server_localfile_own *own = object;

    /* close dir */
    if ( own->dirp != NULL && own->fp == NULL ) {
	int err = closedir( (DIR *)own->dirp );
	if ( err == -1 ) {
	    if ( errno == EBADF ) {
		;
	    }
	    return -1;
	}
    }

    /* close file */
    if ( own->fp != NULL && own->dirp == NULL ) {
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
