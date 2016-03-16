#include <sys/types.h>  /* for stat() */
#include <dirent.h>	/* for opendir() */
#include <sys/stat.h>	/* for stat() */
#include <unistd.h>	/* for stat() */

#include "hpat.h"
#include "server.h"
#include "ppak.h"
#include "addr_t.h"



/*== OBJECT: dpakserver ==*/
dpakserver *new_dpakserver() {
    dpakserver *d = malloc(sizeof(*d));
    if(d != NULL)
        memset(d, 0, sizeof(*d));
    return d;
}

void dpakserver_cleanup( void *dptr ) {
    dpakserver *d = (dpakserver *)dptr;
    if(d == NULL) return;
    if(d->name != NULL)
	free(d->name);

    free(d);
}

int server_name_match_op ( void *serverptr, void *nameptr ) { /* TRUE || FALSE */
    dpakserver *server = (dpakserver *)serverptr;
    char       *name   = (char *)  nameptr;

    if(server == NULL
     ||  name == NULL
     || server->name == NULL) {
	return 0;
    }
    return strcmp(server->name,name) ? 0 : 1 ;	
}
/*--------*/

/*== SERVER: host_dpak == */

size_t host_dpak_read ( void *ptr, size_t rremain, void *media_record, void *frag_addr, void *fragpos ) {
    return 0;
}

/*--------*/



/*== SERVER: host_mem ==*/

dpakstream *host_mem_open ( addr_t *addr, const char *mode, dpakserver *ops ) {
    if(addr == NULL || mode == NULL) return NULL;
}

boundslen_t host_mem_write_trunc ( void *ptr, size_t size, size_t nmemb, dpakstream *ds ) {
    if(ptr == NULL || ds == NULL) { dpakstream_set_error( DS_WRITEFAIL ); return 0; }
    /* because this is a truncate operation
       and because this is a host memory operation
       we simply free the memory address pointed to by ds->dpak->addr->ptr
       and set it to ptr. */

    Dpak *destpak = dpakstream_get_dpak( ds );
    void *dest = dpak_get_addr_ptr( destpak );

    if ( dest != NULL )
	free( dest );

    dpak_set_addr_ptr( destpak, ptr, size * nmemb );

    /* clear error condition */
    dpakstream_set_error( 0 );

    return size * nmemb;
}

boundslen_t host_mem_read ( void *ptr, boundslen_t size, dpakstream *ds ) {
    if(ptr == NULL || addr == NULL ) { dpakstream_set_error( DS_NULLPTR ); return 0; }

    frag_t     * cur_frag;
    addr_t     * addr;
    void       * raddr;
    boundslen_t  rsize;
    
    if(( cur_frag = dpakstream_get_cur_frag( ds )) == NULL ) {
	dpakstream_set_error( DS_NULLPTR );
	return 0;
    }

    if((addr = frag_get_addr( cur_frag )) == NULL) {
	dpakstream_set_error( DS_NULLPTR );
	return 0;
    }

    if((raddr = addr->ptr) == NULL ) {
	dpakstream_set_error( DS_NULLPTR );
	return 0;
    }

    if((rsize = bounds_get_len( addr->bounds, 'x' )) == 0 ) {
	dpakstream_set_error( DS_ZERO_REQUEST );
	return 0;
    }

    /* check to see that fragment bounds aren't outside dpak bounds */
    bounds_t *dbounds = dpak_get_bounds( dpakstream_get_dpak( ds ));
    boundslen_t dpakstart = bounds_get_offset( dbounds, 'x' );
    boundslen_t dpakend   = bounds_get_len( dbounds, 'x' ) + dpakstart;

    bounds_t *fbounds = frag_get_bounds( cur_frag );
    boundslen_t fragstart = bounds_get_offset( fbounds, 'x' );
    boundslen_t fragend   = bounds_get_len( fbounds, 'x' ) + fragstart;

    if(fragstart < dpakstart) {
	fragstart = dpakstart;
    }
    if(fragend > dpakend ) {
	fragend   = dpakend;
    }

    /* ---> WIP adjust size acording to fragend and fragstart */

    size = (rsize < size) ? rsize : size;

    memcpy( ptr, raddr, size );

    return size;
}

size_t host_mem_read2 ( void *ptr, size_t size, media_rec_t *media_rec, frag_addr_t *frag_addr, boundslen_t fragpos ) {

    void      * cur_buf  = frag_get_buf( cur_frag );


    void      * rptr        = ptr;
    boundslen_t rremain     = count * size;
    boundslen_t rread       = 0;    /* initial */

    void      * bufptr      = NULL; /* initial */
    boundslen_t bufsize     = 0;    /* initial */
    boundslen_t bufpos      = 0;    /* initial */
    boundslen_t bufreadsize = 0;    /* initial */
    boundslen_t bufremain   = 0;    /* initial */


    while( rremain > 0 ) {

	if( cur_buf != NULL ) {
	    bufptr      = cur_buf + bufpos;
	    bufsize     = frag_get_size( cur_frag );
	    bufpos      = dpakstream_get_fragpos( ds );
	    bufreadsize = bufsize - bufpos;
	    bufreadsize = ( bufreadsize =< rremain ) ? bufreadsize : rremain;
	    memcpy( rptr, bufptr, bufreadsize );
	    rptr        =+ bufreadsize;
	    rremain     =- bufreadsize;

	}else{
	    bufreadsize = ds->server->read( ptr, rremain, ds );
	    /* ds->server->read can set EOF ds->error */
	    bufremain   = 0;
	}
	fpos    =+ bufreadsize;
	rremain =- bufreadsize;
	rread   =+ bufreadsize;
	
	
	/*    EOFRAG  */
	if( bufremain == 0 ) {
	    cur_frag = frag_get_next( cur_frag ); /* <-- this call enters next_cell if needed */
	    bufpos   = 0;
            /*   EO FRAG CHAIN   */
	    if( cur_frag == NULL ) {
		dpakstream_set_error( ds, EOCHAIN );
		/* update ds and return */
		dpakstream_set_fpos( ds, fpos);
		dpakstream_set_cur_frag( ds, cur_frag );
		dpakstream_set_fragpos( ds, bufpos );

		return rread;
	    }
	    cur_buf = frag_get_buf( cur_frag );
	    continue;
	}
    }
    return rread;
}


/*--------*/

/*== NOTE: naming scheme ==*/
/*
    want to change "localdir" to host_dir
    want to change "localfile" to host_file
*/
/*--------*/

/*== SERVER: localdir ==*/

dpakstream *localdir_open ( addr_t *addr, const char *mode, dpakserver *ops ) {
    if(addr == NULL || mode == NULL) return NULL;

    dpakstream *fp = new_dpakstream();	/* init positions */
    if(fp == NULL) return NULL;

    dpakstream_set_src(fp, (void *)opendir( hpat_str_ptr( addr->pat ) ) );
    dpakstream_set_ops(fp, ops);
    return fp;
}

void localdir_close ( dpakstream *fp ) {
    DIR *dp = (DIR *)dpakstream_get_src(fp);
    if(dp != NULL) closedir(dp);
    dpakstream_cleanup(fp);
}

/*--------*/

/*== SERVER: localfile ==*/

FILE *localfile_fopen(Hpat *addr, const char *mode) {
    if(addr != NULL && mode != NULL) {
	return fopen(hpat_str_ptr(addr), mode);
    }
    fprintf(stderr,"localfile_fopen: addr and mode can't be NULL, abort.\n");
    return NULL;
}

void *localfile_mmap(
	void *addr, size_t len, int prot, int flags,
	int fd, off_t offset) {
    return NULL;
}

dpakstream *localfile_open ( addr_t *a, const char *mode, dpakserver *ops ) {
    return NULL;
}

void localfile_close () {
    return;
}


/*--------*/



/* hiena version 2 */
/* we setup servers internally rather than modular */
list_t *init_serverlib ( ) {
    list_t *first_servl = new_list(NULL);	// garbage collection? TBD
    list_t *cur_servl = first_servl;
    list_t *next_servl = NULL;

    if(first_servl == NULL)
	return NULL;

    /* localdir */
    dpakserver *localdir = new_dpakserver();
    if(localdir == NULL)
	return NULL;

    localdir->name  = strdup("localdir");
    localdir->open  = localdir_open;
    localdir->close = localdir_close;

    list_set_ob ( cur_servl, localdir );
    next_servl = new_list(cur_servl);
    cur_servl  = next_servl;

    if(next_servl == NULL) {
	dpakserver_cleanup(localdir);
	return NULL;
    }

    /* localfile */
    dpakserver *localfile = new_dpakserver();
    if(localfile == NULL) {
	dpakserver_cleanup(localdir);
	return NULL;
    }

    localfile->name  = strdup("localfile");
    localfile->open  = localfile_open;
    localfile->close = localfile_close;
    localfile->fopen = localfile_fopen;
    localfile->mmap  = localfile_mmap;

    list_set_ob ( cur_servl, localfile );

    /* dpakserver */
    //next_servl = new_list(cur_servl);
    //cur_servl  = next_servl;
    /* TBD */


    return first_servl;
}

void serverlib_cleanup ( list_t *servlib ) {
    list_cleanup_all( servlib, dpakserver_cleanup ); 

}
