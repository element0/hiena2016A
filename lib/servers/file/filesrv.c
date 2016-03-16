/* SERVER MODULE: localhost filesystem

architecture: linux


   */
#include <hiena/server.h>	/* generic server header for all hiena server modules */
// ON IT'S WAY OUT... #include "../../src/hdirent.h" /* hiena directory entries */
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

void destroy_own( struct server_localfile_own *own ) {
    if( own == NULL )
	return;
    free(own);
}


void *server_open ( void *addr, const char *mode ) {
    if ( addr == NULL || mode == NULL )
	return NULL;

    struct server_localfile_own *own = new_own();
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

size_t server_read ( void **ptr, size_t size, void *object, struct hiena_server_transaction *h ) {
    if( ptr == NULL || size == 0 || object == NULL || h == NULL )
	return 0;

    struct server_localfile_own *own = object;

    if( own == NULL )
	return 0;

    /* read dir */
    if ( own->dirp != NULL && own->fp == NULL ) {
	printf( "DIR:\n");

	void  *m, *c, *d = h->dir_new();
	struct dirent *dp;
	char  *d_name_var, *child_addr;
	size_t d_name_len, child_addr_len;


	while (( dp = readdir( own->dirp )) != NULL ) {

	    d_name_len 	= strlen(dp->d_name);
	    d_name_var	= strndup(dp->d_name, d_name_len+1);
	    
	    child_addr_len = strlen( own->addr ) + 1 + d_name_len + 1;
	    child_addr     = malloc( child_addr_len * sizeof(char) );

	    snprintf( child_addr, child_addr_len, "%s/%s", own->addr, d_name_var );
	    child_addr[ child_addr_len - 1 ] = '\0';

	    printf( "child_addr: %s\n", child_addr );


	    m = h->map_new_freeagent( d_name_var, d_name_len, "name" ); 
	    c = h->dir_new();

	    h->dir_map_add(c, m);
	    h->dir_addr_set(c, child_addr);

	    h->dir_add(d, c);

	    free(child_addr);
	}
    }

    /* read file */
    if ( own->fp != NULL && own->dirp == NULL ) {
	void *buf = malloc(sizeof(char)*size);
	size_t rread = fread( buf, 1, size, own->fp );
	*ptr = buf;
	return rread;
    }

    return 0;
}

int server_close ( void *object ) {
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
    return 0;
}