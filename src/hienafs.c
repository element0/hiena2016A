
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

typedef fuse_ino_t Hino_t;
typedef uint64_t Hserialno_t;


#include "hienafs_object.c"
#include "hiena_core.c"




void hienafs_destroy ( void * hnfsptr ) {
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


static int hienafs_stat ( struct hiena_file_system * hnfs, fuse_ino_t ino, struct stat *stbuf ) {
    if( hnfs == NULL ) return -1;

    Axpa *ap = accesspathdb_get_accesspath( hnfs->ax, ino );

    if( ap == NULL ) return -1;

    return accesspath_get_stbuf( ap, stbuf );
}

static void hienafs_getattr ( fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
    (void) fi;

    printf("hienafs_getattr: ino %ld\n", ino);

    void *hnfs = fuse_req_userdata( req );
    struct stat stbuf;
    memset(&stbuf, 0, sizeof(stbuf));

    if (hienafs_stat(hnfs, ino, &stbuf) == -1)
	fuse_reply_err( req, ENOENT );
    else
	fuse_reply_attr( req, &stbuf, 1.0 );
}

static void hienafs_lookup ( fuse_req_t req, fuse_ino_t parent, const char *name ) {

    printf("hienafs_lookup: %d/%s\n", parent, name);
    
    struct hiena_file_system * hnfs = fuse_req_userdata( req );
    if( hnfs == NULL ) {
	perror("hienafs_lookup: cant get hiena file system data.\n");
	fuse_reply_err ( req, ENOENT );
	return;
    }

    struct fuse_entry_param e;
    memset(&e, 0, sizeof(e));

    // name --> lookup string cache --> serialno
    Hserialno_t serialno = lookupstringsdb_put_string( hnfs->lookupstrings, name, strlen(name) );

    // TODO: this is inefficient since it adds every single query to the database
    // open to DOS attack
    // should change to only add those queries which have successfully returned results.

    // serialno --> accesspath branches array --> accesspath ino
    Axpa *ap = accesspathdb_get_accesspath( hnfs->ax, parent );
    if( ap == NULL ) {
	perror("hienafs_lookup: can't get access path.\n");
	goto abort_ENOENT;
    }

    if( (e.ino = accesspath_get_branch_ino( ap, serialno )) == 0 ) {


	/* IF YOU CAN'T FIND EXISTING BRANCH, SEARCH WITH PATTERN SCANNERS */
	/* This is the meaty core of hiena... */


	/* get lookup module ops */
	struct hiena_lookup_ops * lookup_ops = lookup_module_get_ops( hnfs->lookup );
	if( lookup_ops == NULL ) {
	    perror("hienafs_lookup: can't get lookup ops.\n");
	    goto abort_ENOENT;
	}

	/* get lookup callbacks */
	struct hiena_lookup_callbacks * lookup_cb = lookup_module_get_callbacks( hnfs->lookup );
	if( lookup_cb == NULL ) {
	    perror("hienafs_lookup: can't get lookup callbacks.\n");
	    goto abort_ENOENT;
	}

	/* prepare 'Hframe' with 'name' and 'lookup_cb' */
	if( hnfs->serverlib == NULL ) {
	    perror("hienafs_lookup: hnfs->serverlib is NULL.\n");
	    goto abort_ENOENT;
	}
	struct hiena_sourceops * server =
	    serverlib_get_serverops( hnfs->serverlib, "mem", strlen("mem")+1 );

	if( server == NULL ) {
	    perror("hienafs_lookup: can't get domain server.\n");
	    goto abort_ENOENT;
	}
	struct hiena_source_callbacks * servcb = serverlib_get_server_callbacks( hnfs->serverlib );
	if( servcb == NULL ) {
	    perror("hienafs_lookup: can't get server callbacks.\n");
	    goto abort_ENOENT;
	}
	struct hiena_lookup_frame * h =
	    lookup_frame_init( lookup_cb, (void *)name, strlen(name), server, servcb);
	if( h == NULL ) {
	    perror("hienafs_lookup: can't initialize lookup frame.\n");
	    goto abort_ENOENT;
	}

	/* open stream for reading */
	h->obj = server->open( h->addr, h->addrlen, "r" ); 
	if( h->obj == NULL ) {
	    perror("hienafs_lookup: can't open address by server.\n");
	    goto abort_ENOENT;
	}


	/* submit 'Hframe' to lookup_ops->yyparse */
	yyscan_t lookup_scanner;
	lookup_ops->yylex_init_extra( h, &lookup_scanner );
	//do{
	    lookup_ops->yyparse( lookup_scanner, h );
	//}while(0);
	lookup_ops->yylex_destroy( lookup_scanner );

	/* close stream */
	server->close( h->obj );

	/* cleanup Hframe */
	lookup_frame_cleanup( h );

	/*--- */


	fuse_reply_err ( req, ENOENT );
	return;
    }

    e.attr_timeout = 1.0;
    e.entry_timeout = 1.0;
    hienafs_stat( hnfs, e.ino, &e.attr );

    fuse_reply_entry( req, &e );
    return;

abort_ENOENT:
    fuse_reply_err ( req, ENOENT );
    return;
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

static void hienafs_opendir ( fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi ) {

    /** Get access path from ino.
      
     */
    struct hiena_file_system * hnfs = fuse_req_userdata( req );
    if( hnfs == NULL ) {
	fuse_reply_err( req, ENOENT );
	return;
    }

    Axpa * ap = accesspathdb_get_accesspath( hnfs->ax, ino );
    if( ap == NULL ) {
	fuse_reply_err( req, ENOENT );
	return;
    }


    /** Refresh access path
        if access path is not in sync, reload scanners.
     */
    if( accesspath_refresh( ap ) == 0 ) {
	perror("hienafs_opendir: can't refresh accesspath.\n");
	fuse_reply_err( req, ENOENT );
	return;
    }
    printf("hienafs_opendir: accesspath refresh OK.\n");



    /* TODO: lock domain handle */

    /** store open directory handle

     */
    fi->fh = (uint64_t)ap;

    fuse_reply_open( req, fi );
}

static void hienafs_readdir ( fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
				struct fuse_file_info *fi ) {

    /** fuse_file_info should now contain an open directory object
     
     */
    Axpa * ap = (Axpa *)fi->fh;


    /* CHECK DOMAIN SYNC STATUS */


    /* PARSE-or-REPARSE DOMAIN IF NEED BE */


    /** get scannerlib

     */
    struct hiena_scannerlib * sl = accesspath_get_parent_scannerlib( ap );
    if( sl == NULL ) {
	perror("hienafs_readdir: can't get accesspath parent's scannerlib.\n");
	return;
    }
    printf("hienafs_readdir: parent scannerlib OK.\n");


    /** get domain

     */
    struct hiena_domaincell * ds = accesspath_get_domain( ap );
    if( ds == NULL ) {
	perror("hienafs_readdir: can't get accesspath's domain.\n");
	return;
    }


    /** run hiena core

     */
    if( hiena( ds, sl ) == 0 ) {
	perror("hienafs_readdir: error with hiena parser.\n");
	return;
    }
    printf("hienafs_readdir: hiena returned OK.\n");


    /* mapping api */


    /* COPY DOMAIN SET'S DIRECTORY ENTRIES TO FUSE DIRECTORY STRUCTURE */


    /* map api */


    /* CREATE FUSE DIRECTORY */
    struct dirbuf b;
    char *d_name;
    fuse_ino_t d_ino;
    memset(&b, 0, sizeof(b));

    /* TEMP */
    d_name = "black hole";
    d_ino  = 3;

    /* OVERRIDE "." AND ".." TO BE ACCESSPATH RELATIVE */
    dirbuf_add( req, &b, ".", ino);
    dirbuf_add( req, &b, "..", accesspath_get_parent_ino( ap ) );
    //dirbuf_add( req, &b, d_name, d_ino );


    reply_buf_limited( req, b.p, b.size, off, size );
    free(b.p);
}

static void hienafs_releasedir ( fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi ) {
    printf("hienafs_releasedir: ino %d\n");

    /* TODO: unlock domain handle */

    fuse_reply_err( req, 0 );
}



/*---- TBD ----*/
static void hienafs_open ( fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi ) {
    //fuse_reply_err( req, EISDIR );
    /* OR */
    fuse_reply_err( req, EACCES );
    /* OR */
    //fuse_reply_open( req, fi );
}

static void hienafs_read ( fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
			 struct fuse_file_info *fi ) {
    (void) fi;

    void * buf = NULL;
    size_t buflen;
    reply_buf_limited( req, buf, buflen, off, size );
}
/*----*/


static struct fuse_lowlevel_ops hiena_oper = {
    /* .init    superceded by hienafs_init() first thing in main */
    .destroy    = hienafs_destroy,
    .lookup	= hienafs_lookup,
    .getattr	= hienafs_getattr,
    .opendir    = hienafs_opendir,
    .readdir	= hienafs_readdir,
    .releasedir = hienafs_releasedir,
    .open	= hienafs_open,
    .read	= hienafs_read,
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
