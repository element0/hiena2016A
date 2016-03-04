/*_HIENA_SCANNER_H_*/
#ifndef _HIENA_SCANNER_H_
#define _HIENA_SCANNER_H_

#include <stdio.h>

typedef struct  hiena_parse_packet  Ppak;
typedef struct  hiena_scannerlib    scanlib;
typedef struct  hiena_scannerserver scannerserver;
typedef struct  hiena_scannerserver_ops Hscannerops_t;

typedef void   *yyscan_t;
typedef struct  hiena_scanner_payload Hsp;

scannerserver  *hiena_scanner_create ();			/* TODO: rename new_scanner() */
void            hiena_scanner_cleanup( scannerserver * );
void            do_scanner           ( scannerserver *, Ppak *, scanlib * );
void            do_filter            ( scannerserver *, Ppak * );
void            do_scanners          ( Ppak *, scanlib * );

typedef struct hiena_scannerserver
{
    /* COMPILER VERSION 1 and newer */
    int compiler_version;

    /* COMPILER VERSION 1 */
    int    name_str_dim;/*number of name strings*/
    char **name_str;/*name strings*/
    int   *name_table;/*name table*/
    int   *fsm_table;/*state machine table*/
   
    /* GENERAL */
    char          *name;
    void          *dl;	        // for dlopen()
    scanlib       *slib;
    const list_t  *servers;	/* these are *inteded* immutable - host relative, global */
    /* TBD: maybe we should link a master filesystem object here? */

    /* COMPILER VERSION 0 */
    Hscannerops_t *op;   // function pointers

    /* first development version is to use a linked list */
    /* just to get us up and running */
    struct hiena_scannerserver *prev;
    struct hiena_scannerserver *next;

    /* reminder: flexible array members must be at end of struct */
    char	  *allowed_child_scanners[];
}scannerserver;


#endif /*!_HIENA_SCANNER_H_*/
