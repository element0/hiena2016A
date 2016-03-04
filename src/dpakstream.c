
/*== OBJECT: dpakstream ==*/
/* patterned after FILE object */

dpakstream *new_dpakstream ( void ) {
    dpakstream *ds = malloc(sizeof(*ds));
    if(ds != NULL)
	memset(ds, 0, sizeof(*ds));		/* sets all positions to 0 */
    return ds;
}

dpakstream *dpakstream_cleanup ( dpakstream **ds) { /* always return NULL */
    if(*ds == NULL) return NULL;
    if(*ds->bounds != NULL) bounds_cleanup( *ds->bounds );
    free(*ds), ds = NULL;
    return NULL;
}


static int dpakstream_set_dpak ( dpakstream *ds, Dpak *dpak ) { /* TRUE | FALSE */
    return ds == NULL ? 0 : ds->dpak = dpak, 1;
}

static int dpakstream_set_mode ( dpakstream *ds, const char *mode ) { /* TRUE | FALSE */
    return ds == NULL || ds->dpak == NULL ? 0 : ppak_set_mode( ds->dpak, mode ), 1;
}

void dpakstream_set_src ( dpakstream *ds, void *src ) {
    if(ds == NULL) return;
    ds->src = src;
}
static int dpakstream_set_filter ( dpakstream *ds, void *filter ) { /* TRUE | FALSE */
    return ds == NULL ? 0 : ds->filter = (scannerserver *)filter, 1;
}
static int dpakstream_set_server ( dpakstream *ds, dpakserver *server ) { /* TRUE | FALSE */
    return ds == NULL ? 0 : ds->server = server, 1;
}
void dpakstream_set_ops ( dpakstream *ds, dpakserver *ops ) {
    if(ds == NULL) return;
    ds->ops = ops;
}
static int dpakstream_set_cur_frag ( dpakstream *ds, frag_t *fr ) { /* TRUE | FALSE */
    /* reminder: cur_frag is a reference, no need to free */
    return ds == NULL ? 0 : ds->cur_frag = fr, 1;
}


Dpak *dpakstream_get_dpak ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return ds->dpak;
}

const char *dpakstream_get_mode ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    if(ds->dpak == NULL) return NULL;
    return ppak_get_mode( ds->dpak );
}

bounds_t *dpakstream_get_fpos ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return ds->fpos;
}

void *dpakstream_get_filter ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return (void *)ds->filter;
}
dpakserver *dpakstream_get_server ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return ds->server;
}
Dpak *dpakstream_get_src ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return ds->src;
}
frag_t *dpakstream_get_cur_frag ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return ds->cur_frag;
}

bounds_t *dpakstream_get_fragpos ( dpakstream *ds ) {
    if(ds == NULL) return NULL;
    return ds->fragpos;
}

bounds_t *dpakstream_get_bounds ( dpakstream *ds ) {
    /* Gets bounds of entire stream. */
    return NULL;
}







enum ds_whence { DS_SEEK_SET, DS_SEEK_CUR, DS_SEEK_END };

/* TBD: a multi-dimensional version of dsseek */
bounds_t *dpak_dsseek_bounds ( dpakstream *ds, bounds_t offsets, int whence ) {
    return NULL;
}

off_t dpak_dsseek ( dpakstream *ds, off_t offset, int whence ) {
    /* mimics 'lseek' */
    off_t cur_offset = 0;

    if(ds == NULL) return -1;
    switch(whence) {
	case DS_SEEK_SET:
	    cur_offset = offset;
	    break;
	case DS_SEEK_CUR:
	    cur_offset = bounds_get_offset( ds->pos, 'x' ) + offset;
	    break;
	case DS_SEEK_END:
	    cur_offset = bounds_get_len( ds->bounds, 'x' ) + offset;
	    /* reminder: don't update ds->bounds until write */
    }
    if(cur_offset < 0 ) cur_offset = 0;
    bounds_set_offset( ds->pos, 'x', cur_offset );
    return cur_offset;
}

/*== INCLUDE: ( inline ) ==*/
#include "fragbuf.c"
#include "filter.c"
/*--------*/
    struct dsread_stack_frame {
	scannerserver *filter;
	filterframe   *fframe;
	int num_frags_created;
	struct dsread_stack_frame *next;
    };

static struct dsread_stack_frame dsread_stack_frame_init ( scannerserver *ft, filterframe *ff, int numf, struct sdread_stack_frame *nx ) {

    struct dsread_stack_frame *stack = malloc(sizeof(*stack));

    stack->filter = ft;
    stack->fframe = ff;
    stack->num_frags_created = numf;
    stack->next   = nx;
    return stack;
}







size_t dpak_dsread2 ( void *ptr, size_t size, size_t count, dpakstream *ds ) {

    if ( ptr == NULL || ds == NULL ) dpakstream_set_error( DS_NULLPTR ), return 0;

    size_t rread   = 0;
    size_t rremain = size * count; 

    dpakserver *frag_server = NULL;
    scannerserver *filter   = NULL;
    Hsp *fframe             = NULL;

    struct dsread_stack_frame *stack_frame = new_stack_frame();
    struct dsread_stack_frame *new_frame   = NULL;

    stack_frame->frag    = dpakstream_get_frag( ds );
    stack_frame->fragpos = dpakstream_get_fragpos( ds );
    stack_frame->dpak    = dpakstream_get_dpak( ds );
    stack_frame->pos     = dpakstream_get_fpos( ds );

    frag_t  *frag = stack_frame->frag;
    off_t fragpos = stack_frame->fragpos;
    off_t media_pos;
    off_t media_len = rremain;



/* filter */
    if ( frag == NULL ) {
	filter = dpak_get_filter( stack_frame->dpak );
	fframe = dpak_get_filter_frame( stack_frame->dpak );
	stack_frame->frag = filter->op->yyparse( fframe->lexer, fframe, media_len );
	/* returns first of the next frags parsed */
	fframe_snapshot = filter_make_snapshot( fframe );
	dpak_push_filter_frame( ds, fframe_snapshot );

	frag = stack_frame->frag;
    }

    while ( frag != NULL && rremain > 0 ) {

/* frag */
	frag_server = dpak_get_server( stack_frame->dpak );

	if ( frag_server == NULL ) { 		/* frag_media_record is a dpak */

	    /* create new stack frame */
	    new_frame = new_stack_frame();

	    new_frame->frag = NULL;
	    new_frame->fragpos = 0;
	    new_frame->dpak = frag->media_record;
	    new_frame->pos  = frag->addr->offset + fragpos;

	    push_frame( &stack_frame, new_frame );

	} else { 
/* server */
	    media_rec = frag->media_record;
	    media_pos = frag->addr->offset + fragpos;
	    media_len = frag->addr->len - fragpos;
	    
	    rread   =+ frag_server->read( ptr, media_len, media_rec, media_pos );
	    rremain =- rread;
	}

	/* set frag and fragpos */
	fragmap_locate_position(
		&stack_frame->frag,
		&stack_frame->fragpos,
		stack_frame->dpak,
		stack_frame->pos
		);

	frag = stack_frame->frag;
	fragpos = stack_frame->fragpos;
/* filter */
	if ( frag == NULL ) {
	    filter = dpak_get_filter( stack_frame->dpak );
	    fframe = dpak_get_filter_frame( stack_frame->dpak );
	    stack_frame->frag = filter->op->yyparse( fframe->lexer, fframe, media_len );
	    /* returns first of the next frags parsed */
	    fframe_snapshot = filter_make_snapshot( fframe );
	    dpak_push_filter_frame( ds, fframe_snapshot );

	    frag = stack_frame->frag;
	}

    } /* end while ( frag != NULL ) */

    dpakstream_incr_fpos( ds, rread );
    dpakstream_set_frag( ds, frag );
    dpakstream_set_fragpos( ds, fragpos );
    return 0;
}

boundslen_t dpak_dsread (void *ptr, boundslen_t count, boundslen_t size, dpakstream *ds) {
    /* patterned after fread() */
    /* using boundslen_t instead of size_t */

    if(ptr == NULL || ds == NULL || ds->dpak == NULL) {
	dpakstream_set_error( ds, DS_NULLPTR );
	return 0;
    }
    

    frag_t      * frag     = dpakstream_get_next_frag( ds, NULL);
    boundslen_t   fragpos  = 0;


    if ( ! frag_check_sync( frag )) {
	/* TODO: handle sync; */
    }
    

    boundslen_t rread = 0;
    boundslen_t rremain = count * size; 
    enum dpakstream_error_t dserrno;

    while ( rremain > 0 ) {
	rread =+ fragbuf_read( ptr, count, size, ds );
	dserrno = dpakstream_get_error( ds );
	if ( dserrno == DS_EOFRAGBUF ) {
	    dsfilter_read( ptr, count, size, ds ); 
	    dserrno = dpakstream_get_error( ds );
	}
	switch ( dserrno ) {
	    case DS_NO_FILTER:
	    case DS_NULLPTR:
	    case DS_ZERO_REQUEST:
	    default:
		rremain = 0;
		continue;
	}
	rremain =- rread;
    }

    return rread;
}

int dpak_dserror ( dpakstream *ds ) {
    /* used by Hsp callback structure */
    /* returns non-zero if ds->error is set */
    return dpakstream_get_error( ds );
}

void dpak_dsclearerr ( dpakstream *ds ) {
    dpakstream_set_error( ds, 0 );
}


/* SUBDIVIDING EXISTING CELLS */

dscell_t *dsprep_writecell ( void *ptr, size_t size ) {
    /* ptr must already be vetted before calling */

    frag_t *f = new_frag();
    if(f == NULL) {
	fprintf(stderr, "hiena: dpak_dswrite_trunc: can't malloc frag, abort.\n");
	return NULL;
    }
    if(frag_set_buf(f, ptr, size) == -1) {
	frag_cleanup(f);
	return NULL;
    }
    if(frag_set_buf_cleanup(f, free) == -1 ) {
	if(frag_set_buf_cleanup(f, NULL) == -1 ) {
	    frag_cleanup(f);
	    return NULL;
	}
    }

    /* create bounds for fragment */
    bounds_t *b = new_bounds(0);
    if(b == NULL) {
	frag_cleanup(f);
	return NULL;
    }
    if( (bounds_add_axis(b,'x',0,size)) == -1 ) {
	frag_cleanup(f);
	return NULL;
    }
    if( (frag_set_bounds(f, b)) == -1 ) {
	bounds_cleanup(b);
	frag_cleanup(f);
	return NULL;
    }
    /* now we have a new fragment which has the write-stuff in its buffer. */

    dscell_t *c = new_dscell();
    if(c == NULL) {
	frag_cleanup(f);
	return NULL;
    }
    /* a brand new cell can use dscell_init_frag() */
    if( dscell_init_frag(c, f) != 0 ) {
	fprintf(stderr, "hiena: dpak_dswrite_trunc: can't init frag, abort.\n");
	frag_cleanup(f);
	dscell_cleanup(c);
	return NULL;
    }
    return c;
}

int dsvalidate_writecell ( dscell_t *dc ) {
    return 0;
}


/* WRITE FUNCTIONS */

static ssize_t dpak_dswrite_over ( dpakstream *ds, size_t size, void *ptr ) {
    /* take data at "ptr"
       encapsulate in a 'write-cell'
    */
    return 0;
}
static ssize_t dpak_dswrite_insert ( dpakstream *ds, size_t size, void *ptr ) {
    return 0;
}
static ssize_t dpak_dswrite_append ( dpakstream *ds, size_t size, void *ptr ) {
    return 0;
}

static size_t dpak_dswrite_trunc ( void *ptr, size_t size, size_t nmemb, dpakstream *ds ) { /* NMEMB WRITTEN */
    /* file mode already vetted if dpak_dswrite() is previous stack frame. */

    /* follows "man 3 fwrite" somewhat, sets error via dpakstream_set_error() */
    if(ptr == NULL || ds == NULL) { 
	dpakstream_set_error( ds, DS_NULLPTR );
	return 0;
    }

    /* MAKE WRITE-CELL */
    dscell_t *c = dsprep_writecell( ptr, size * nmemb );


    /* GET CURRENT CELL -- OF WHOLE DOMAIN */
    Dpak *d = dpakstream_get_dpak( ds );
    if(d == NULL) {
	dscell_cleanup(c);
	dpakstream_set_error( ds, DS_NULLPTR );
	return 0;
    }

    dscell_t *domain_cell = dpak_get_val_cell( d ); 	/* TBD: should this cause a sync? */
    if(domain_cell == NULL) {
	/* MAKE WHOLE-DOMAIN CELL */
	/* make empty cell and add to dpak */
	if( (domain_cell = new_dscell()) == NULL) {
	    dscell_cleanup(c);
	    return 0;
	}
	if( dpak_set_val_cell( d, domain_cell ) == -1 ) {
	    dscell_cleanup(c);
	    return 0;
	}
    }

    /* STAGE 2 - VALIDATION */

    /* validate write cell as entire domain (because this is a truncate) */
    ds->filter->write_trunc( c->first_frag, ds );


    /* STAGE 4 - CONFIRM */

    /* COMMIT WRITE-CELL TO REPLACE WHOLE DOMAIN CELL */
    /* tmp-cell will be preserved via write-cell's undo chain */
    if( dpak_set_val_cell( d, c ) == -1 ) {
	dscell_cleanup( c );
	return 0;
    }
    
    /* add domain-cell to write-cell's undo stack */
    if( dscell_set_undo( c, domain_cell ) == -1 ) {
	dscell_cleanup( c );
	return 0;
    }

    /* RETURN SUCCESS */
    return size * nmemb;
}

size_t dpak_dswrite( void *ptr, size_t size, size_t nmemb, dpakstream *ds ) { /* nmemb written */
    if( ptr == NULL || ds == NULL || size * nmemb == 0 ) return 0;

    const char *mode = dpakstream_get_mode( ds );

    switch 0
    {
	case strncmp("w",  mode, 2):
	    return dpak_dswrite_trunc( ptr, size, nmemb, ds );
	case strncmp("w+", mode, 3):
	    return dpak_dswrite_trunc( ptr, size, nmemb, ds );
	case strncmp("r",  mode, 2):
	    fprintf(stderr, "hiena: dpak_dswrite: file mode not implemented.\n");
	    return 0;
	case strncmp("r+", mode, 3):
	    fprintf(stderr, "hiena: dpak_dswrite: file mode not implemented.\n");
	    return 0;
	case strncmp("a",  mode, 2):
	    fprintf(stderr, "hiena: dpak_dswrite: file mode not implemented.\n");
	    return 0;
	case strncmp("a+", mode, 3):
	    fprintf(stderr, "hiena: dpak_dswrite: file mode not implemented.\n");
	    return 0;
	default:
	    fprintf(stderr, "hiena: dpak_dswrite: unrecognized file mode.\n");
	    return 0;
    }
}


int dpak_dsclose ( dpakstream *ds ) { /* '0' || EOF */
    /* pattern after 'fclose' */
    /* contrary to 'fclose' we don't set a global errno */

    ppak_decr_opencount( dpakstream_get_dpak(ds) );
    dpakstream_cleanup( ds );

    return EOF;
}


dpakstream *dpak_dsopen ( Dpak *p, const char *mode ) {
    if(p == NULL || mode == NULL) return NULL;

    dpakstream *ds = new_dpakstream();

    if (  ! dpakstream_set_dpak    ( ds, p ),
       || ! dpakstream_set_server  ( ds, ppak_get_server( p ))
       || ! dpakstream_set_filter  ( ds, (void *)ppak_get_filter( p ))
       || ! dpakstream_set_cur_frag( ds, ppak_get_first_frag( p ))
       || ! dpakstream_set_mode    ( ds, mode ))
    {
	return dpakstream_cleanup( &ds );
    }

    ppak_incr_opencount( p );

    return ds;
}




/*--------*/

