/** @file dcel.c

  Domain Cell Server Module

  this is a 'snafu' file server module
  which provides access to the internal 'dcel' structure type.
  this will get used a lot during the hiena parse event.

 */

#include <stdlib.h>	/* malloc */
#include <string.h>	/* memset */

#include "../../../include/hiena/server_module.h"	/* generic header for all hiena server modules */
#include "../../../src/domaincell.h"

struct domaincell_protocol_addr;
struct hiena_domaincell;

struct server_domaincell_fh {
    struct domaincell_protocol_addr * paddr;
    const char * mode;
    dcel * dcel;
    void * srcfh;
    size_t fpos;
};

static struct server_domaincell_fh * server_new_fh () {
    struct server_domaincell_fh * fh = malloc(sizeof(*fh));
    memset(fh, 0, sizeof(*fh));
    return fh;
}

static void server_destroy_fh ( struct server_domaincell_fh * fh ) {
    if( fh == NULL ) return;
    if(fh->mode != NULL )
	free(fh->mode);
    free( fh );
}

static int server_validate_dcel ( void * dcelptr ) {
    dcel * dcel = (dcel *)dcelptr;
    if( dcel->op == NULL
     || dcel->op->try_lock == NULL
     || dcel->op->unlock   == NULL ) {
	return 0;
    }
    if( dcel->serverlib  == NULL
     || dcel->serversign == NULL
     || dcel->addr       == NULL ) {
	return 0;
    }
    return 1;
}


/* this operation conforms to "hiena/server_module.h" */

void * server_open ( void * protocol_addr, size_t protocol_addr_len, const char * modein,
	struct hiena_source_callbacks * h) {

    if ( protocol_addr == NULL || modein == NULL ) return NULL;
    
    (struct domaincell_protocol_addr *) protocol_addr;
    /* has
	   dcel,
	   off,
	   len
    */

    (void) protocol_addr_len;

    const char * mode = strndup( modein, HIENA_FILEMODE_CHAR_MAX );
    if( mode == NULL )  return NULL;


    /* TEST mode */
    /* check to see if we can open with mode */

    dcel * dcel = protocol_addr->dcel;
    if( server_validate_dcel( dcel ) == 0 ) {
	goto abort;
    }
    if( dcel->op->trylock( mode, NULL ) == 0 )  goto abort;


    /* outline: */
    /* OPEN dcel's source stream
       SEEK position 'addr->off'
       NEW  server_domaincell_fh
       RTRN server_domaincell_fh */


    /* OPEN a file handle on dcel's source stream */
    struct hiena_source_stream_handle * srcfh = h->hiena_open( &dcel->addr, 0, mode, h );
    if( srcfh == NULL ) goto abort2;

    /* SEEK position 'addr->off' */
    if( h->hiena_seek( srcfh, protocol_addr->off, SEEK_SET) != 0 ) {
	h->hiena_close( srcfh );
	goto abort2;
    }

    /* NEW server_domaincell_fh */
    struct server_domaincell_fh * fh = server_new_fh();
    if(fh == NULL)  goto abort2;

    fh->srcfh = srcfh;
    fh->dcel = dcel;
    fh->mode = mode;
    fh->paddr = protocol_addr;

    /* RTRN server_domaincell_fh */
    return fh;

abort2:
    dcel->op->unlock( dcel, mode, NULL );
abort:
    free( mode );
    server_destroy_fh( fh );
    return NULL;
}

size_t server_read ( void * ptr, size_t size, void * fhptr, struct hiena_source_callbacks * h ) {
    if( ptr == NULL || size == 0 || fh == NULL || h == NULL ) return 0;

    struct server_domaincell_fh * fh = fhptr;
    if( fh == NULL ) return 0;

    /* select src file handle from open 'fh' */
    void * srcfh = fh->srcfh;
    if( srcfh == NULL ) return 0;

    /* constrain read size to protocol address boundaries */
    size_t max_remaining = fh->paddr->len - fh->fpos;
    if( size > max_remaining )
	size = max_remaining;

    /* use srcfh's server to read */
    size_t rread = h->hiena_read( ptr, size, srcfh, h );

    /* advance fpos */
    fh->fpos =+ rread;

    return rread;
}

int server_close ( void * fhptr, struct hiena_source_callbacks * h ) {
    struct server_domaincell_fh * fh = fhptr;

    if( fh == NULL || h == NULL || fh->dcel == NULL )
	return -1;

    /* fh->dcel functions should have already been validated during
       'server_open' */
    fh->dcel->op->unlock( fh->dcel );

    /* close subordinate source file handle */
    h->hiena_close( fh->srcfh, h );
   
    server_destroy_fh( fh );

    return 0;
}
    


/* end file dcel.c */
