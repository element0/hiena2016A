/*_HIENA_ADDR_T_H_*/
#ifndef _HIENA_ADDR_T_H_
#define _HIENA_ADDR_T_H_


/*== OBJECT: addr_t ==*/
#include <sys/types.h> /* off_t, size_t */

void    addr_t_cleanup( addr_t *a );

typedef struct hiena_parse_packet_addr
{
    /* SERVER RELATIVE ADDRESS */
    void     *ptr;
    size_t    size;


    /* SCANNER RELATIVE ADDRESS */
    bounds_t *bounds;
    int       bounds_dim;

    /* MEMORY: 'bounds' free'd by Ppak,
       do not include any malloc'd items inside 'bounds' */
    /* "bounds" is defined by the scanner during map object creation
       and expresses a scanner-relative address in the data
       sourced by the parent ppak.  */

    /* What the hell is this? */
    frag_t   *parfrag;
    
}addr_t;
/*--------*/

#endif /*!_HIENA_ADDR_T_H_*/
