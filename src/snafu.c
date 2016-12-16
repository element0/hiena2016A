/** @file
 * 
 * this is the main "snafu" system mounter utility
 *
 * Right now, I'm correcting the inodes to access frames.
 */



#define FUSE_USE_VERSION 30

#include <fuse_lowlevel.h>
#include <stdio.h>

#include "cosmos_grid.h"
#include "cosmos_aframe.h"


struct snafu_fs {
	void *grid;
	fuse_ino_t rootino;
};

static struct snafu_fs *snafu_init ( char *mountpoint ) {
	struct snafu_fs * fs = malloc(sizeof(fs));
	if( fs == NULL ) return NULL;
	memset(fs, 0, sizeof(fs));

	/* init a grid session.  NULL implies current user */

	void *grid = grid_init( NULL );

	/* make absolute URL "file://<host>/<absolutepath>" */

	char *url = grid_filepath_to_URL( mountpoint );
	
	/* (NULL would be URL to bind with) */

	grid_add_url( grid, url, NULL );

	fs->grid = grid;
	return fs;
};

static void snafu_lookup (fuse_req_t req, fuse_ino_t parent, const char *name) {
	struct fuse_entry_param e;
	memset(&e, 0, sizeof(e));

	struct snafu_fs *fs = fuse_req_userdata( req );

	struct cosmos_aframe *aframe = grid_lookup( fs->grid, parent, name );
	aframe_stat( aframe, &e.attr );

	e.attr_timeout = 1.0;
	e.entry_timeout = 1.0;
	e.ino = &e.attr.s_ino;
	ruse_reply_entry( req, &e );
}

static void snafu_getattr ( fuse_req_t req, fuse_ino_t ino,
				struct fuse_file_info *fi ) {
	(void) fi;
	struct stat stbuf;
	memset(&stbuf, 0, sizeof(stbuf));

	struct snafu_fs *fs = fuse_req_userdata( req );

	struct cosmos_aframe *aframe = grid_get_inode( fs->grid, ino );

	if ( aframe_stat( aframe, &stbuf ) == -1 )
		fuse_reply_err(req, ENOENT);
	else
		fuse_reply_attr(req, &stbfu, 1.0);
}

struct dirbuf {
	char *p;
	size_t size;
};

static void dirbuf_add ( fuse_req_t req, struct dirbuf *b, const char *name,
			 fuse_ino_t ino) {
	struct stat stbuf;
	size_t oldsize = b->size;
	b->size += fuse_add_direntry( req, NULL, 0, name, NULL, 0);
	b->p     = (char *) realloc(b->p, b->size);
	memset( &stbuf, 0, sizeof(stbuf));
	stbuf.st_ino = ino;
	fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf,
			  b->size);
}

#define min(x, y) ((x) < (y) ? (x) : (y))

static int reply_buf_limited ( fuse_req_t req, const char *buf, size_t bufsize,
		              off_t off, size_t maxsize) {
	if (off < bufsize)
		return fuse_reply_buf( req, buf + off,
				       min(bufsize - off, maxsize));
	else
		return fuse_reply_buf(req, NULL, 0);
}

static void snafu_readdir ( fuse_req_t req, fuse_ino_t ino, size_t size,
			    off_t off, struct fuse_file_info *fi ) {
	(void) fi;

	struct cosmos_aframe *aframe = grid_get_inode( ino );

	struct stat stbuf;
	memset(&stbuf, 0, sizeof(stbuf));

	if( aframe_stat( aframe, &stbuf ) == -1 ) {
		fuse_reply_err( req, ENOENT );
		return;
	}

	if( S_ISDIR(stbuf.st_mode) ) {
		struct dirbuf b;

		memset(&b, 0, sizeof(b));
		/* add all directory entries here */
		
		struct dirent *prev;
		struct dirent *next;
		while( aframe_readdir_r( aframe, prev, &next ) ) {
			dirbuf_add(req, &b, next->d_name, next->ino);
			prev = next;
		}	
		reply_buf_limited(req, b.p, b.size, off, size);
		free(b.p);
	}
}

static void snafu_open ( fuse_req_t req, fuse_ino_t ino,
		         struct fuse_file_info *fi ) {
	struct cosmos_aframe *aframe = grid_get_inode( ino );
	struct stat stbuf;
	memset(&stbuf, 0, sizeof(stbuf));

	if( aframe_stat( aframe, &stbuf ) == -1 ) {
		fuse_reply_err( req, ENOENT );
		return;
	}

	if( S_ISDIR(stbuf.st_mode) ) {
		fuse_reply_err(req, EISDIR);
	}
	fi->fh = aframe_fopen( aframe, fi->flags );
	fuse_reply_open(req, fi);
}

static void snafu_read ( fuse_req_t req, fuse_ino_t ino, size_t size,
		         off_t off, struct fuse_file_info *fi ) {
	const char *buf;
	size_t rsize = aframe_fread( &buf, size, fi->fh );
	fuse_reply_buf( req, buf, rsize );
	free( buf );
}

static void snafu_release ( fuse_req_t req, fuse_ino_t ino,
		      struct fuse_file_info *fi ) {
	aframe_fclose( fi->fh );
}


static struct fuse_lowlevel_ops snafu_ops = {
	.lookup		= snafu_lookup,
	.gettr		= snafu_getattr,
	/* .opendir	= snafu_opendir, */
	.readdir	= snafu_readdir,
	/* .releasedir	= snafu_releasedir, */
	/* .fsyncdir	= snafu_fsyncdir, */
	.open		= snafu_open,
	.read		= snafu_read,
	.release	= snafu_release,
};

int main (int argc, char *argv[]) {
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_chan *ch;
	char *mountpoint;
	int err = -1;

	if(fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1) {

		/* this may need to go before fuse_mount */
		struct snafu_fs *fs = snafu_init(mountpoint);

		/* fuse mount must happen after snafu_init() in order
		   to preserve the contents of the underlying mountpoint.
		 */
		if((ch = fuse_mount(mountpoint, &args)) != NULL) {
			struct fuse_session *se;

			se = fuse_lowlevel_new(&args, &snafu_ops,
					sizeof(snafu_ops), fs);
			if (se != NULL) {
				if (fuse_set_signal_handlers(se) != -1) {
					fuse_session_add_chan(se, ch);

					/* Block until ctrl+c or fusermount -u */
					err = fuse_session_loop(se);

					fuse_remove_signal_handlers(se);
					fuse_session_remove_chan(ch);
				}
				fuse_session_destroy(se);
			}
			fuse_unmount(mountpoint, ch);
		}
		fuse_opt_free_args(&args);
	}

	return err ? 1 : 0;
}
