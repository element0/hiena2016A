
#define FUSE_USE_VERSION 30

#include <config.h>

#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "hienafs_object.c"




void hiena_destroy ( void * hnfsptr ) {
    struct hiena_file_system * hnfs = hnfsptr;
/*
    hienafs_unload_domain_servers( );
    hienafs_unload_rql_module( hnfs );
    hienafs_cleanup_domain_packets( hnfs );
    hienafs_cleanup_lookup_strings( hnfs );
    hienafs_cleanup_access_paths( hnfs );
*/
    hienafs_cleanup( hnfs );
}


static int hiena_stat ( void *hnfs, fuse_ino_t ino, struct stat *stbuf ) {
    return -1;
    /* alters "stbuf" */

	/*
    if( hienafs_access_path_verify( hnfs, ino )) {

	stbuf->st_ino = ino;
	stbuf->st_mode = hienafs_access_path_st_mode( hnfs, ino );
	stbuf->st_nlink = hienafs_access_path_st_nlink( hnfs, ino );
	return 0;

    } else {
	return -1;
    }
    return 0;
*/

}

static void hiena_getattr ( fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {

    void *hnfs = fuse_req_userdata( req );
    struct stat stbuf;

    (void) fi;

    memset(&stbuf, 0, sizeof(stbuf));
    if (hiena_stat(hnfs, ino, &stbuf) == -1)
	fuse_reply_err( req, ENOENT );
    else
	fuse_reply_attr( req, &stbuf, 1.0 );
}

static void hiena_lookup ( fuse_req_t req, fuse_ino_t parent, const char *name ) {
    
    void *hnfs = fuse_req_userdata( req );

    struct fuse_entry_param e;
    memset(&e, 0, sizeof(e));

    // name --> hnfs{ string cache } --> serialno
    uint64_t serialno = 1; //hienafs_string_serialno_lookup( hnfs, name );

    // serialno --> hnfs{ axpa['parent']{ branches }} --> axpano | 0
    e.ino = 1; //hienafs_access_path_branch_lookup( hnfs, parent, serialno );

    if ( e.ino == 0 ) {
	fuse_reply_err ( req, ENOENT );
    }else{
	e.attr_timeout = 1.0;
	e.entry_timeout = 1.0;
	hiena_stat( hnfs, e.ino, &e.attr );

	fuse_reply_entry( req, &e );
    }
}

/*---- from "fuse/example/hello_ll.c" ----*/
struct dirbuf {
	char *p;
	size_t size;
};

static void dirbuf_add ( fuse_req_t req, struct dirbuf *b, const char *name, fuse_ino_t ino )
{
	struct stat stbuf;
	size_t oldsize = b->size;
	b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);
	b->p = (char *) realloc(b->p, b->size);
	memset(&stbuf, 0, sizeof(stbuf));
	stbuf.st_ino = ino;
	fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf, b->size);
}

#define min(x, y) ((x) < (y) ? (x) : (y))

static int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
			     off_t off, size_t maxsize)
{
	if (off < bufsize)
		return fuse_reply_buf(req, buf + off, min(bufsize - off, maxsize));
	else
		return fuse_reply_buf(req, NULL, 0);
}

/*----*/

static void hiena_readdir ( fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
				struct fuse_file_info *fi ) {

    (void) fi;
    void *hnfs = fuse_req_userdata( req );



	/*
    if ( !hienafs_access_path_verify( hnfs, ino ) ) {

*/	fuse_reply_err( req, ENOTDIR );
    /*
    }else{
	struct dirbuf b;
	void *cursor = hienafs_access_path_branches_new_cursor( hnfs, ino );
	char *d_name;
	fuse_ino_t d_ino;

	memset(&b, 0, sizeof(b));

	// for each in axpa{ branches }
	while ( ( each = hienafs_access_path_next_branch( cursor )) != 0 )
	{
	    d_name = hienafs_access_path_d_name( hnfs, each );
	    d_ino  = hienafs_access_path_d_ino( hnfs, each );
	    dirbuf_add( req, &b, d_name , d_ino );
	}
    
	reply_buf_limited( req, b.p, b.size, off, size );
	free(b.p);
	hienafs_access_path_branches_cleanup_cursor( cursor );
    }
	*/
}



/*---- TBD ----*/
static void hiena_open ( fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi ) {
    //fuse_reply_err( req, EISDIR );
    /* OR */
    fuse_reply_err( req, EACCES );
    /* OR */
    //fuse_reply_open( req, fi );
}

static void hiena_read ( fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
			 struct fuse_file_info *fi ) {
    (void) fi;

    void * buf = NULL;
    size_t buflen;
    reply_buf_limited( req, buf, buflen, off, size );
}
/*----*/


static struct fuse_lowlevel_ops hiena_oper = {
    /* .init    superceded by hienafs_init() first thing in main */
    .destroy    = hiena_destroy,
    .lookup	= hiena_lookup,
    .getattr	= hiena_getattr,
    .readdir	= hiena_readdir,
    .open	= hiena_open,
    .read	= hiena_read,
};

int main ( int argc, char *argv[] ) {

    struct hiena_file_system *hienafs = hienafs_init();

    int num_source_args = 0;
    
    if ( (num_source_args = hienafs_parse_cmdline( hienafs, argc, argv )) == 0 ) {
	hienafs_cleanup( hienafs );
	return -1;
    }
    struct fuse_args args = FUSE_ARGS_INIT(argc - num_source_args, argv);
    struct fuse_chan *ch;
    char *mountpoint;
    int err = -1;

    if(fuse_parse_cmdline( &args, &mountpoint, NULL, NULL ) != -1 &&
        (ch = fuse_mount( mountpoint, &args )) != NULL ) {
	
	struct fuse_session *se;

	/* pass custom data structure in arg[4] */
	se = fuse_lowlevel_new( &args, &hiena_oper, sizeof(hiena_oper), hienafs );

	if(se != NULL) {
	    if(fuse_set_signal_handlers( se ) != -1 ) {
		fuse_session_add_chan( se, ch );

		err = fuse_session_loop( se );

		fuse_remove_signal_handlers( se );
		fuse_session_remove_chan( ch );
	    }
	    fuse_session_destroy( se );
	}
	fuse_unmount( mountpoint, ch );
    }
    fuse_opt_free_args( &args );

    return err ? 1 : 0;
}
