/* to be included inline from dpakstream.c */

boundslen_t fragbuf_read ( void *ptr, boundslen_t count, boundslen_t size, dpakstream *ds ) {
    
    if (ptr == NULL || ds == NULL) { dpakstream_set_error( ds, DS_NULLPTR ); return 0; }
    if (count * size == 0)         { dpakstream_set_error( ds, DS_ZERO_REQUEST); return 0; }

    
    
    frag_t         * frag         = NULL;
    dpakserver     * frag_server  = NULL;
    boundslen_t      fragpos      = 0;
    frag_addr_t    * frag_addr    = NULL;
    media_record_t * media_record = NULL;


    boundslen_t rread   = 0;
    boundslen_t rremain = count * size;

    while ( rremain > 0 ) {
	if ( frag = dpakstream_get_next_frag( ds, frag ) == NULL ) {
	    dpakstream_set_error( ds, DS_EOFRAGBUF );
	    break;
	}
	if (  ( media_record = frag_get_media_rec( frag )) == NULL
	   || ( frag_addr    = frag_get_addr( frag ))      == NULL 
	   || ( frag_server  = frag_get_server( frag ))    == NULL
	   || ( frag_server->read )                        == NULL )
	{
	    dpakstream_set_error( ds, DS_NULLPTR );
	    break;
	}

	fragpos = dpakstream_get_fragpos( ds );

	rread = frag_server->read( ptr, rremain, media_record, frag_addr, fragpos );

	if ( rread > 0 ) dpakstream_incr_fpos( ds, rread );
	rremain =- rread;
    }

    return rread;
}
