/*== NOTE: dpakstream.h ==*/
// This file is included inline from ppak.h
// it depends on being included from there.
/*--------*/

/*== OBJECT: dpakstream ==*/
/* used by dpak_f* functions and by dpakserver functions.
   saves stream position, fragment , and filter state.
   
   the main API for this object is dpak_f*.
 */

enum dpakstream_error_t {
    DS_EOFRAG, DS_EOFRAGBUF, DS_NO_FRAGS, DS_FRAGBUF_DIRTY, DS_FRAGBUF_EMPTY,
    DS_EOCHAIN, DS_NULLPTR, DS_ZERO_REQUEST,
    DS_NO_FILTER,
    DS_WRITELOCK, DS_WRITEFAIL
};

struct hiena_dpak_stream
{
    Dpak          *dpak;	/* referenced.  this is the domain. */
    const char    *mode;	/* referenced.  */
    /* filter manipulates the fragments inside dpak's domain cell */
    boundslen_t    fpos;        /* owned.. */
    bounds_t      *bounds;	/* owned.. */
    /* FILTER STAGE */
    frag_t        *cur_frag;	/* referenced, must not free */
    boundslen_t    fragpos;	/* owned.. */

    scannerserver *filter;	/* referenced.. same as dpak->filter */
    Hsp		  *fstate;      /* referenced.. from dpak->fstate[*] */
    /* fstate has "fp" or "ds" produced by server */

    /* SERVER STAGE */
    dpakserver    *server;	/* referenced.. */
    void	  *fp;		/* opened by server -- can be a dpakstream */

    /* ERROR STUFF */
    enum dpakstream_error_t error;

};
/*--------*/
