/*_HIENA_SERVER_H_*/
#ifndef _HIENA_SERVER_H_
#define _HIENA_SERVER_H_

#include <sys/types.h>	/* off_t, size_t */


typedef struct hiena_serverops            dpakserver;
typedef struct hiena_dpak_stream	  dpakstream;
typedef struct hiena_parse_packet_addr    addr_t;
typedef struct ppak_list_element	  list_t;
typedef struct hiena_pattern_buffer       Hpat;

dpakserver *new_dpakserver      ();
void        dpakserver_cleanup  ( void * );
int         server_name_match_op( void *, void * );
dpakserver *determine_server    ( addr_t *, list_t * );
list_t     *init_serverlib      ();
void        serverlib_cleanup   ( list_t * );

addr_t *new_addr_t    ();
void    addr_t_cleanup( addr_t *a );


/*== OBJECT: addr_t ==*/
typedef struct hiena_parse_packet_addr
{
    /* DPAK SERVER RELATIVE ADDRESS */
    Hpat     *pat;
    /* AKA */
    Hpat     *srvaddr;
    /* AKA */
    void     *ptr;
    size_t    size;

    Dpak     *par;

    /* SCANNER DERIVED ADDRESS */
    bounds_t *bounds;
    int       bounds_dim;
    /* MEMORY: 'bounds' free'd by Ppak,
       do not include any malloc'd items inside 'bounds' */
    /* "bounds" is defined by the scanner during map object creation
       and expresses a scanner-relative address in the data
       sourced by the parent ppak.  */
    frag_t   *parfrag;
    /* DEPRICATE ME */
    Ppak     *parent;	/* this exact pointer is also contained in val.loc.parent */
    			/* val.loc subject to retirement */
			/* refers to the originating object:
			   if current object is derivative, parent refers to original.
			   if current object is a mapob,
			   parent is the object that owns the map. */
}addr_t;
/*--------*/


/*== OBJECT: dpakserver ==*/
typedef struct hiena_serverops dpakserver;
struct hiena_serverops {
    
    FILE       *(*fopen) (Hpat *, const char *);	/* := !NULL */
    void       *(*mmap)  (void *, size_t, int, int, int, off_t);
    /* version 2 hiena */
    char       *name;
    dpakstream  *(*open)  (addr_t *, const char *mode, dpakserver *);
    void         (*close) (dpakstream *);
    boundslen_t *(*read)  ( void *, boundslen_t count, boundslen_t size, dpakstream *ds );
    boundslen_t *(*write) (dpakstream *ds, void *buf, boundslen_t size);
};
/*--------*/


#endif /*!_HIENA_SERVER_H_*/
