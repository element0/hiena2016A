/* filter.c */
/* to be included inline from dpakstream.c */

size_t dsfilter_write_trunc ( dscell_t *f, dpakstream *ds ) {
    if(f == NULL || ds == NULL) {
	dpakstream_set_error( DS_NULLPTR );
	return 0;
    }
    size_t sizewritten = 0;


    /* dswrite chain - STAGE 2 - validation */
    /* write_trunc is the easiest write-mode.
       we just replace the entire domain with the new fragment "f". */

    /* filter step 1: encode */

    void *buf;
    size_t bsize;

    ds->filter->encode( &buf, &bsize, f->buf, f->size );


    /* filter step 2: validate */

    dscell_t *c = dsprep_writecell( buf, bsize );

    Dpak *tempak = new_dpak( ds->dpak );
    if( tempak == NULL ) {
	dpakstream_set_error( ds, DS_WRITEFAIL );
	goto cleanup1;
    }

    if( !dpak_set_val_cell( tempak, c )
     || !ppak_set_addr_ptr( tempak, buf, bsize )
     || !ppak_set_server_name( tempak, "host_mem", ds->filter->servers )
     || !ppak_set_filter_name( tempak, "default-filter", ds->filter->slib ) )
    {
	dpakstream_set_error( ds, DS_WRITEFAIL );
	goto cleanup2;
    }

    /* bc truncate, no filter restart checkpoints will be used */
    /* instead a new Hsp will be created just for validation */

    Hsp *temph = hsp_init( tempak );
    if ( hsp_init_scanner( temph, ds->filter) == 0 ) {
	dpakstream_set_error( ds, DS_WRITEFAIL );
        goto cleanup2;
    }
    if (!ds->filter->validate( temph )) {
	dpakstream_set_error( ds, DS_WRITEFAIL );
	goto cleanup3;
    }


    /* dswrite chain - STAGE 4 - server write */

    /* bc truncate mode, we don't need to server->fseek() before write */
    /* -- ergo, fseek to zero implied in server->write_trunc() */

    sizewritten = ds->server->write_trunc( buf, bsize, 1, ds );
    /* server->write_trunc sets ds->error, pass through untouched */

    if (sizewritten != bsize && dpakstream_get_error( ds ) != DS_WRITEFAIL ) {
	/* TODO: handle this situation.  where encoded size is sent to the server
	   but the server writes a different size to its media.
	   there's obviously an error if that's the case, how should we proceed? */
	/* the media has changed so we really should reload from the media */
	dpakstream_set_error( ds, DS_MUST_RELOAD_FROM_SERVER );
	/* leave sizewritten */
	goto cleanup3;
    }

    /* translate sizewritten back to decoded size */
    sizewritten = f->size;

cleanup3:
    hsp_cleanup( temph );
cleanup2:
    ppak_cleanup( tempak );
cleanup1:
    dscell_cleanup( c );
    return sizewritten;
}

boundslen_t dsfilter_read ( void *ptr, boundslen_t count, boundslen_t size, dpakstream *ds ) {

    if (ptr == NULL || ds == NULL)  { dpakstream_set_error( ds, DS_NULLPTR ); return 0; }
    if (count * size == 0)  { dpakstream_set_error( ds, DS_ZERO_REQUEST ); return 0; }


    scannerserver *filter = dpakstream_get_filter( ds );
    if ( filter == NULL )  {
	dpakstream_set_error( ds, DS_NO_FILTER );
	return 0;
    }
    
    
    /* TODO: handle sync; */



    boundslen_t rremain = count * size;
    boundslen_t endpos  = dpakstream_get_fpos( ds ) + rremain;
    boundslen_t filterpos = filter_get_fpos( filter );

    boundslen_t rread   = 0;

    /* filter material leading up to end of current request */

    while( rremain > 0 ) {
	rremain = endpos - filter_get_fpos( filter );
	rread   =+ filter->parse( dpakstream_get_filter_state( ds ), rremain );
	/* important: filter->parse should not advance ds->fpos */
	/*  filter->parse maps and buffers fragments inside ds->dpak */
    }

    return 1;
}
