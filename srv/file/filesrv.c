/* SERVER MODULE: localhost filesystem

architecture: linux

   */
#include "../server.h"	/* generic server header for all hiena server modules */
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


void *server_open ( void *addr, const char *mode, struct hiena_transaction *h ) {
    if ( addr == NULL || mode == NULL || h == NULL )
	return NULL;

    struct server_localfile_own *own = new_own();
    destroy_own( h->own );
    h->own = (void *)own;

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
        destroy_own( h->own );
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
            destroy_own( h->own );
	    h->own = NULL;
	}

	own->dirp = dirp;
	own->fp   = NULL;

	return (void *)dirp;
    }
    
    /* if file type */
    if ( S_ISREG(own->sb.st_mode) ) {
	FILE *fp = fopen( (const char *)addr, mode );
	own->fp   = fp;
	own->dirp = NULL;

	if( fp == NULL ) {
            destroy_own( h->own );
	    h->own = NULL;
	}

	return (void *)fp;
    }

    return NULL;
}

size_t server_read ( void **ptr, size_t size, void *object, struct hiena_transaction *h ) {
    if( ptr == NULL || size == 0 || object == NULL || h == NULL )
	return 0;

    struct server_localfile_own *own = h->own;

    if( own == NULL )
	return 0;

    /* read dir */
    if ( own->dirp != NULL && own->fp == NULL ) {
	struct dirent *direntp;
	int chid = 0;

	size_t child_addr_len;
	char *child_addr = NULL;

	printf( "DIR:\n");
	while ((direntp = readdir( own->dirp )) != NULL) {
	    chid = h->new_child( h->id );
	    h->child_set_prop( "name", direntp->d_name, chid );

	    child_addr_len = strlen(own->addr) + 1 + strlen(direntp->d_name) + 1;
	    child_addr = malloc(child_addr_len*sizeof(char));
	    snprintf( child_addr, child_addr_len, "%s/%s", own->addr, direntp->d_name );
	    child_addr[child_addr_len-1] = '\0';
	    h->child_set_addr( child_addr, chid );
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

int server_close ( void *object, struct hiena_transaction *h ) {
    if ( object == NULL || h == NULL )
	return -1;
    
    struct server_localfile_own *own = h->own;
    if ( own == NULL ) return -1;
    /* close dir */
    if ( own->dirp != NULL && own->fp == NULL ) {
	int err = closedir( (DIR *)object );
	if ( err == -1 ) {
	    if ( errno == EBADF ) {
		;
	    }
	    return -1;
	}
    }

    /* close file */
    if ( own->fp != NULL && own->dirp == NULL ) {
	int err = fclose( (FILE *)object );
	if ( err == EOF ) {
	    if ( errno == EBADF ) {
		;
	    }
	    return -1;
	}
    }

    destroy_own( h->own );
    h->own = NULL;

    return 0;
}
