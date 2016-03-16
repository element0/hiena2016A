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

#include "addr_t.h"


/*== OBJECT: dpakserver ==*/
typedef struct hiena_serverops dpakserver;
struct hiena_serverops {
    
    FILE       *(*fopen) (Hpat *, const char *);	/* := !NULL */
    void       *(*mmap)  (void *, size_t, int, int, int, off_t);
    /* version 2 hiena */
    char        *name;
    dpakstream  *(*open)  (addr_t *, const char *mode, dpakserver *);
    void         (*close) (dpakstream *);
    boundslen_t *(*read)  ( void *, boundslen_t count, boundslen_t size, dpakstream *ds );
    boundslen_t *(*write) (dpakstream *ds, void *buf, boundslen_t size);
};
/*--------*/



#endif /*!_HIENA_SERVER_H_*/
