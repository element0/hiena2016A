/*_HIENA_PPAK_H_*/
#ifndef _HIENA_PPAK_H_
#define _HIENA_PPAK_H_

#include <stdio.h>	/* FILE */
#include <stdint.h>	/* uint64_t */
#include <sys/types.h>	/* off_t */

typedef struct hiena_pattern_buffer       Hpat;
typedef struct hiena_scanner_payload	  Hsp;
typedef struct hiena_access_path	  Axpa;
typedef struct hiena_parse_packet	  Dpak;
typedef struct hiena_parse_packet	  Ppak;
typedef struct hiena_parse_packet_map_key Ppkey;
typedef struct hiena_parse_packet_value   Dpak_stream_internal;
typedef struct hiena_parse_packet_domain_location     Ppbounds;
typedef struct hiena_parse_packet_map_positional_node Mapn;
typedef struct hiena_dpak_stream	  dpakstream;
typedef struct hiena_serverops            dpakserver;
typedef struct hiena_server_io_queue      ioqueue_t;
typedef struct ppak_bound_element	  boundl;
typedef struct ppak_bounds_ob 		  bounds_t;
typedef struct ppak_fragbuf_element	  frag_t;
typedef struct ppak_list_element	  list_t;
typedef struct hiena_parse_packet_addr	  addr_t;
typedef int    				  Htok_t;


list_t *new_list      ( list_t * );
list_t *new_list_t    ( void * );
int     list_t_cleanup( list_t * );
int	list_verify   ( list_t * );
list_t *list_get_next ( list_t * );
void    list_set_ob   ( list_t *, void *ob );
void   *list_get_ob   ( list_t * );
int     list_t_add_ob ( list_t *, void *ob );
void   *list_get_match( list_t *, int(*)(void *, void *), void * );
void    list_cleanup_all( list_t *, void(*)(void *) );
int     list_t_cleanup_garbage( list_t * );

bounds_t *new_bounds         ( int );
int       bounds_cleanup     ( bounds_t * );
boundl   *bounds_get_axis_ptr( bounds_t *, int );
off_t     bounds_get_len( bounds_t *, int );
int       bounds_add_axis    ( bounds_t *, int, off_t, off_t );
int       bounds_pointcpy    ( bounds_t *, bounds_t * );
bounds_t *bounds_cmp         ( bounds_t *, bounds_t * );


Ppak   *new_ppak            ( Ppak * );
void    ppak_cleanup        ( Ppak * );
void    ppak_cleanup_garbage( Ppak * );
void    ppak_retain         ( Ppak * );
void    ppak_release        ( Ppak * );
void    ppak_incr_opencount ( Ppak * );
void    ppak_decr_opencount ( Ppak * );
int     ppak_import_key     ( Ppak *, Ppkey * );
int     ppak_set_addr_str   ( Ppak *, const char * );
void    ppak_set_addr_par   ( Ppak *, Ppak * );
addr_t *ppak_get_addr       ( Ppak * );
int     ppak_verify_addr    ( Hpat * );	/* TODO: replace Hpat with addr_t */
int     ppak_determine_server( Ppak *, list_t * );
int     ppak_set_server     ( Ppak *, dpakserver * );
dpakserver *ppak_get_server ( Ppak * );
int     ppak_verify_server  ( dpakserver * );
int     ppak_verify_val     ( Ppak * );
int     ppak_set_val_str    ( Ppak *, const char * );
char   *ppak_get_str        ( Ppak * );
int     ppak_set_strv       ( Ppak *, int, char *argv[] );
int     ppak_set_buf        ( Ppak *, Hpat * );
list_t *ppak_get_first_prop ( Ppak * );
list_t *ppak_get_next_prop  ( Ppak *, list_t * );
list_t *ppak_get_first_child( Ppak * );
list_t *ppak_get_next_child ( Ppak *, list_t * );
list_t *ppak_get_next_row   ( Ppak *, list_t * );
int     ppak_import_val     ( Ppak *, Dpak_stream_internal * );

/* TODO: replace Hpat's with frag buffers */
Ppak   *new_ppak_with_hpat    ( Hpat *, Ppak * );
Ppak   *new_ppak_from_strv    ( int, char *argv[], Ppak * );
Ppak   *new_ppak_from_str     ( const char *, Ppak * );
Ppak   *new_ppak_with_addr_str( const char *, Ppak * );

int     ppak_is_dud    ( Ppak * );

void    ppak_set_filter( Ppak *, void * );
scannerserver *ppak_get_filter( Ppak * );

/* TODO: overhaul or depricate */
int     ppak_set_val_in_parent  ( Ppak *, uint64_t, uint64_t );	/* TODO: overhaul or depricate */
Mapn   *hiena_get_posnode       ( Ppak *, Ppbounds * );
int     hiena_posnode_add_object( Mapn *, Ppak * );
Ppak   *hiena_posnode_get_token ( Mapn *, Ppkey * );
/*******************************/

FILE   *ppak_fopen( Ppak *, char * );

frag_t *new_frag();
void    frag_set_prev       ( frag_t *, frag_t * );
void    frag_set_next       ( frag_t *, frag_t * );
void    frag_set_bounds     ( frag_t *, bounds_t * );
void    frag_set_buf        ( frag_t *, void * );
void    frag_set_buf_cleanup( frag_t *, void (*)(void *) );
frag_t *frag_get_prev       ( frag_t * );
frag_t *frag_get_next       ( frag_t * );
bounds_t *frag_get_bounds   ( frag_t * );
void   *frag_get_buf        ( frag_t * );
void    frag_cleanup        ( frag_t * );
void    frag_cleanup_chain  ( frag_t * );

dpakstream *new_dpakstream();
void        dpakstream_cleanup   ( dpakstream * );
void        dpakstream_set_dpak  ( dpakstream *, Ppak * );
void        dpakstream_set_src   ( dpakstream *, void * );
void        dpakstream_set_filter( dpakstream *, void * );
void        dpakstream_set_server( dpakstream *, dpakserver * );
void        dpakstream_set_ops   ( dpakstream *, dpakserver * );		/* Depricate? */
Ppak       *dpakstream_get_dpak  ( dpakstream * );
void       *dpakstream_get_filter( dpakstream * );
dpakserver *dpakstream_get_server( dpakstream * );
Ppak       *dpakstream_get_src   ( dpakstream * );
frag_t     *dpakstream_get_cur_frag( dpakstream * );

size_t      dpak_dsread ( void *, size_t, dpakstream * );
int         dpak_dsclose( dpakstream * );
dpakstream *dpak_dsopen ( Ppak * );

int	 ppak_set_bounds ( Ppak *, int, off_t, off_t );
void     ppak_set_map_buf( Ppak *, off_t, void * );
Ppak    *ppak_make_ob    ( int, Hsp * );

Mapn    *mapnode_add_connection( Mapn *, Ppak * );
void     bounds_grow           ( bounds_t **, bounds_t * );
int      ppak_add_mapob        ( Ppak *, Ppak *, int );
int      ppak_add_prop_hsp     ( Ppak *, Ppak *, Hsp * );
int      ppak_add_child        ( Ppak *, Ppak * );
int      ppak_add_child_hsp    ( Ppak *, Ppak *, Hsp * );
int	 ppak_list_map         ( Ppak *, int );

void     ppak_kill_all_frags     ( Ppak * );
void     ppak_set_storage_updated( Ppak *, int );
int      ppak_refresh            ( Ppak *, Axpa *a, Hsp *h );


/*== OBJECT: bounds_t ==*/

typedef off_t boundslen_t;

typedef struct ppak_bound_element boundl;
struct ppak_bound_element
{
    int   axis;
    boundslen_t offset;
    boundslen_t len;
};

enum bound_cmp_status
{
    BNDS_SAME, BNDS_INSIDE, BNDS_OUTSIDE, BNDS_INTERSECT, BNDS_DIFF
};

struct ppak_bounds_ob {
    int     c;      /* num array elements */
    boundl *a;      /* array of boundry axi */
    int     pointmatch; /*{ $$ = 0 if no matching performed yet,
			         1 if all offsets have matched so far,
				 2 if one or more offsets have not matched
			}*/
    int     status; /* status from bounds_cmp()
    		          'i' inside
		          'o' outside
		          '=' exact match
		    */
};
/*--------*/


/*== INLINE INCLUDE: dpakstream.h ==*/
#include "dpakstream.h"
/*--------*/


/*== INLINE INCLUDE: frag_t.h ==*/
#include "frag_t.h"
/*--------*/


/*== OBJECT: dscell_t ==*/
typedef struct dpak_write_cell
{
    frag_t *first_frag;		/* frag's and cells are mutually exclusive. one set must be NULLs. */
    frag_t *last_frag;
    /* | */
    dscell_t *first;
    dscell_t *last;

    bounds_t *bounds;

    dscell_t *prev;
    dscell_t *next;

    dscell_t *undo;
    dscell_t *redo;

}dscell_t;

/*--------*/

/*== STRUCTURES: parse packet ==*/

typedef struct ppak_list_element list_t;
struct ppak_list_element
{
    list_t *prev;
    list_t *next;

    void *ob;
};

typedef struct hiena_parse_packet_domain_location Ppbounds;
typedef struct hiena_parse_packet_domain_location
{
    Ppak *parent;		/* := !NULL */


    /* useful? */
    long unsigned int first_line;
    long unsigned int first_column;
    long unsigned int last_line;
    long unsigned int last_column;

    /* uint64_t          offset; */
    off_t             offset;
    /* uint64_t          len;*/	/* := ( > 0)  */
    off_t             len;
   
    struct ppak_bound_element *array;
    void     *xbounds;		
    /* stub for multi-dimensional bounds */

    /* DEPRECATING */
    Ppak *next;
}Ppbounds;

typedef struct hiena_parse_packet_map_positional_node Mapn;
typedef struct hiena_parse_packet_map_positional_node Ppposn;
typedef struct hiena_parse_packet_map_positional_node
{
    bounds_t *bounds;


    /* WIP */
    int       numob;
    list_t   *ob;

    int       numcon;
    list_t   *con;		/* connections to adjacent map nodes */
    /* RETIRE */
    Mapn     *connection;	/* connections to adjacent map nodes */
    				/* following one of these makes up a parse path */

   
    /* OLD STYLE */
    off_t pos;
    Ppposn *next_posnode;	/* next positional node */
    Ppak   *first_aligned;	/* first mapobject in chain aligned to node */

    /* GARBAGE COLLECTION */
    Mapn *prev_garbage;
    Mapn *garbage;


}Ppposn;




typedef struct hiena_parse_packet_value		/* aka "VALUE" */
{

    dscell_t *cell;		/* domain cell -- contains frag map, frag's can contain buffers */ 

}Dpak_stream_internal;

typedef struct hiena_parse_packet_map_key		/* aka "KEY" */
{
    scannerserver    *scanner;
    Htok_t         tokid;	/* this object's grammar rule id number */
    Hpat          *name;        /* replaces 'tokname' */
    Hpat          *tokfull;	/* this object's grammar rule
				   unique global digital signature */
    /*----- RETIRE ----- */
    Hpat          *tokname;	/* this object's grammar rule id name */

}Ppkey;



typedef struct hiena_parse_packet
{
    /* HOUSEKEEPING */
    int      retain;		/* for future convenience. 2015-07-06 */
    void     *lock;		/* for writes */
    const char *mode;		/* for dpakstream */
    Ppak     *garbage_prev;
    Ppak     *garbage_next;
    dpakstream_error_t *error;	/* for dpakstream */

    /* sync flags */
    int       storage_updated;
    int       map_changed;
    int       scanlib_changed;


    /* FORMATION ATTACH POINTS: different ways to cluster parse packets */
    Ppak     *next_union_member;
    Ppak     *next_derivative;
    Ppak     *next_alternative;
    Ppak     *prev_undo;
    Ppak     *next_redo;
    


    char    *hostname;		/* corresponds to gethostname(2) - owned by hiena utility object */
    long int cosmos_vlan_id;		/* internal vlan network id */
    long int cosmos_vlan_node_id;	/* internal node id */
    /* above id's translate to a network address.
       both id's are relative to the cosm
       hiena-cosmos must translate them to be localhost relative
       the idea is that a dpak from one host can be transferred to another host
       and the vlan addresses will remain the same,
       even though the hosts may be on two different physical subnets.
       */

    

    char          serversign[16];	/* this server ID could replace direct pointer */
    dpakserver    *server;	/* "server" serves to "filter" */
    int		   syncstate;
    ioqueue_t     *ioq;		/* io queue for unreachable server */


    addr_t   *addr;	/* "addr" is passed to "server" */
    			// if addr!=NULL && parent!=NULL, is addr in parent?
    			/* addr can be NULL and zero'd out,
       			   in which case, "val" contains original data. */


    void     *filter;		/* provided by scanner which generated the dpak *//* ref */
    list_t   *fframe;		/* list type can house rewind points *//* owned */
    /* "filter" generates "val" */


    dscell_t *val;		/* contains stream value
				   via domain cells and frag buffers */
    				/* "val" is generated by "filter" */
    int	     open_count;	/* implies 'is_open' with reference counts */




    /* SCANNER NOTE */
    /* scanners are provided by the access path structure
     * they are noted here because of how they 
     * affect this structure.
     * each scanner is provided at the access path node level
     * inputs from a 'dpakstream'
     * dpakstream relies on 'filter'
     * -- provided by the scanner which generated the dpak
     * outputs to property map (rooted at "first_posnode")
     * outputs to child map (rooted at "first_child_posnode")
     */

    /* PROPERTY MAP */
    /* map of attributes, properties, features, etc.
     * which are NOT children in the directory hierarchy.
     */
    Mapn     *prop_root;
    list_t   *first_prop;

    
    /* CHILD MAP */
    /* map of children in the directory hierarchy */
    list_t   *first_child;

    
    /* MAP MEMBERSHIP ASPECT */
    Ppak     *next_mapob;	/* link to next consecutive map object (parse packet) */
    Ppposn   *next_posnode;	/* link to next consecutive positional node  */
    Ppak     *next_aligned;	/* link to next map object aligned
				   at same start bound (parse packet) */
    Ppak     *next_unordered;	/* for unordered or abstract values */

    Ppkey    key;	/* contains scanner-creator reference */
    			/* "key" is paired with "val" when ppak is a property */
    
}Ppak;
/*--------*/



#endif /* !_HIENA_PPAK_H_ */
