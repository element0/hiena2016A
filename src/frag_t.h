/* included inline from ppak.h */

/*== OBJECT: frag_t ==*/
typedef struct dpak_write_cell dscell_t;
struct ppak_fragbuf_element
{
    void     *buf;
    size_t    size;		/* size_t coresponds with dswrite functions */

    void     *media_record;

    /* frag_addr */
    bounds_t *bounds;
    addr_t   *addr;		/* addr served by parent dpak's server */

    void     *server;		/* this servs the data at fragment */
    				/* if fragment is buffered, this is buffer */

    frag_t   *prev;		/* non-null except for border fragments */
    frag_t   *next;		/* non-null except for border fragments */

    dscell_t *prev_cell;	/* NULL except for border fragments */
    dscell_t *next_cell;	/* NULL except for border fragments */

    void     *(*buf_cleanup) (void *); 
    void     *ops;

    /* NIXME */
    bounds_t *pos;
    bounds_t *offset;
};
/*--------*/
