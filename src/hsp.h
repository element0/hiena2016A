/*_HIENA_SCANNER_PAYLOAD_H_*/
#ifndef _HIENA_SCANNER_PAYLOAD_H_
#define _HIENA_SCANNER_PAYLOAD_H_

#include <stdio.h>
#include <sys/types.h>	/* off_t */
#include "bounds_t.h"

typedef struct hiena_scanner_payload Hsp;
typedef struct hiena_scanner_payload_callback_ops Hspops;
typedef struct hiena_scannerserver   scannerserver;
typedef struct non_variadic_argument nova_t;
typedef struct rql_expr_element      expr_t;
typedef struct hiena_parse_packet    Ppak;
typedef struct hiena_access_path     Axpa;
typedef struct hiena_scannerlib      scanlib;
typedef void                        *yyscan_t;

Hspops *hsp_init_globals     ();
void    hsp_globals_cleanup  ();

void    hsp_clear_nova       ( Hsp * );
void    hsp_clear_sql_rescol ( Hsp * );
int     hsp_set_nova         ( Hsp *, void * );
int     hsp_sql_set_rescol   ( Hsp *, void * );
int     hsp_sql_select_core  ( Hsp * );
int     hsp_sql_set_selection( Hsp *, int );
int     hsp_sql_getchar      ( Hsp * );
int     hsp_sql              ( Hsp *, const char * );
char   *hsp_getenv           ( const char * );
Htok_t  hsp_gettok           ( Hsp * );

Hspops *hsp_ops_create();
Hspops *hsp_ops_init();
int     hsp_ops_cleanup      ( Hspops * );

Hsp    *new_hsp     ();
void    hsp_cleanup ( Hsp * );
Hsp    *hsp_init    ( Ppak * );

/* TODO: rework/rename */
int     hsp_set_target( Hsp *h, Ppak * );
int     hiena_scanner_session_rewind( Hsp * );
/***********************/

Axpa *hsp_get_axpa       ( Hsp * );
int   hsp_set_axpa       ( Hsp *, Axpa * );
int   hsp_set_scanlib    ( Hsp *, scanlib * );
int   hsp_set_scanner_ptr( Hsp *, scannerserver * );
int   hsp_init_scanner   ( Hsp *, scannerserver * );
int   hsp_set_scanner    ( Hsp *, const char * );		/* TODO: rename? hsp_load_scanner */
int   hsp_open_stream    ( Hsp * );
void  hsp_close_stream   ( Hsp * );
Axpa *hsp_execute        ( Hsp * );







/* the hiena scanner payload callback operations
   keep a list of function pointers available to 
   a c-compiled scannerserver.
 */
typedef struct hiena_scanner_payload_callback_ops 
{
    /*-- SCANNING FUNCTIONS --*/
    Htok_t (*gettok)(Hsp *);
    size_t (*dsread)     (void *, size_t, size_t, Hsp *);
    int    (*ferror)     (Hsp *);
    void   (*clearerr)   (Hsp *);
    
    /*-- MAPPING FUNCTIONS --*/
    Ppak *(*make_ob)    (int tokid, Hsp *);
    int   (*set_bounds) (Ppak *, int axis, off_t offset, off_t len);
    void  (*set_buf)    (Ppak *, off_t len, void *buf);

    int   (*add_prop)   (Ppak *, Ppak *, Hsp *);
    int   (*add_child)  (Ppak *, Ppak *, Hsp *);
    int   (*merge_ob)   (Ppak *, Ppak *);

    /*-- WIP: SQL PARSING SUPPORT --*/
    int   (*nova)	(Hsp *, void *var);
    int   (*sql)	(Hsp *, const char *);
    int   (*sql_getchar)(Hsp *);

    /*-- CONTEXT FUNCTIONS --*/
    char *(*getenv)(const char *);

}Hspops;



/* HIENA SCANNER PAYLOAD
   =====================
   The Hsp holds the working objects for the scanner
   during a parse.  It also contains a structure of 
   function pointers to the API which the scanner code
   is allowed to use.

   The Hsp ties together the aspects of Hiena-Cosmos:
   The Parse Packet the Scannerserver
   the Access Path, and the Request Language.

   
   ACCESS PATH
   ===========
   The access path holds the complete context under which
   the current parse can operate.  The items in this context
   are available to the current scanner, and can include
   scannerserver library, environment variables, host sandbox,
   and parent access Parse Packet.  The access Parse Packet,
   can contain an entire subdomain available to the scanner,
   and this subdomain can be separate from the Hsp's source
   packet.


   SOURCE PACKET
   =============
   The Source Packet is the Parse Packet which will be 
   examined by the scannerserver.  It contains the property map
   and child map which the scanner will implicity map to.
   The scanner is in charge of mapping its objects independently
   of the source packet, but with each mapping call, Hiena
   connects those objects to the source packet's maps.


   SCANNER
   =======
   The purpose of the Hsp is to hold parse-in-progress state
   and to provide a set of API operations for a single scanner
   to use during a parse.  There is a one-to-one relationship
   between an Hsp and a Scanner.  The scanner object is also
   used by Hiena to expose a set of function calls and data which 
   the individual scanner provides.  Hiena uses translation tables
   from the scanner object to transform the 'tokid' of each
   map object from a local to a global id.  The scanner also
   provides the essential 'parse' function.

   Sidenote: The scanner may be a Hiena-compiled set of state
   machines, in which case the parse function will be implemented
   by the builtin Hiena Engine rather than by a shared object.
   The 'parse' function will be set by the scanner library 
   loading function, and is transparent to the Hsp.


   REGISTERS
   =========
   There are a number of registers, Ppak pointers, which 
   a request scanner can use to build derivative objects.
   As an example, a Request Scanner must set a Target.
   If an operation is to be performed between map objects,
   use the Source and Destination registers.


   CUSTOM SCANSTATE WORKSPACE
   ==========================
   If you want to create your own structure to hold parse-in-progress
   state, you can attach it to the scandata pointer.


 */

typedef struct non_variadic_argument {
    char   *type;
    void   *var;
    nova_t *next;
}nova_t;


typedef struct rql_expr_element {
    int     type;
    void   *val;
    expr_t *next;
}expr_t;


typedef struct hiena_scanner_payload
{
    /*** PRIVATE VARIABLES ***/

    /* result or garbage collector */
    Ppak *parseroot;

    /* scanner state */
    yyscan_t       lexer;
    unsigned long int bytes_read; 	/* needed to calculate cur_tok_start */
    off_t cur_location;		 	/* used by parse_packet.c apparently */
    char *cur_tok_name;
    int   cur_tok_id;
    off_t cur_tok_start;
    off_t cur_tok_len;
    char *cur_tok_text;
    Ppak *last_added;		/* by put */
    Ppak *last_read;		/* by gettok */
    bounds_t *fpos;		/* by dsfilter_read */

    /* file pointer */
    FILE *fp;		/*:= !NULL */
    /* REPLACE ABOVE WITH... */
    void * ds;
    //dpakstream *ds;
    /* this requires rewriting bison generated scanners
       YY_INPUT to read from a dpakstream */
    

    /*** internal or external? ***/
    Axpa *axpa;

    /*** EXTERNALLY VISIBLE FUNCTIONS ***/
    Hspops *op;		/*:= !NULL */
    

    /*** EXTERNALLY VISIBLE VARIABLES ***/ 
    /* set through methods in 'op' */

    /* RQL lookup workspace */
    //rq *rq;		/*:= !NULL */			

    /* SQL scanner workspace */
    nova_t *first_arg;	/* non-variadic-argument-function argument linked list */
    nova_t *last_arg;
    nova_t *cur_arg;	/* temp. used by sql_getchar() callback */
    char *strp;		/* sql_getchar() */
    char *cur_arg_strp; /* sql_getchar() */

    /* SQL: micro-select statement workspace */
    Ppak *selection;
    /* FROM clause: table_or_subquery_list */
    Ppak *sql_targ;

    /* WHERE clause: expr */
    expr_t *expr_stack;
    expr_t *first_expr;
    expr_t *last_expr;

    /* retiring */
    char *sql_expr_lval;
    char *sql_expr_rval;
    int   sql_expr_op;



    /* result_expr_list */
    char *sql_col_expr;
    Ppak *temp_row;
    Ppak *result_table;
    nova_t *first_result_col;
    nova_t *last_result_col;
    nova_t *cur_result_col;





 
    /* data source reference */
    /* (reference pointer only.  do not free.) */
    Ppak *target;	/*:= !NULL */


    /* scanner reference */
    /**** CHANGE this to a 'slib_entry' structure ****/
    /* (simpler thing to do is change 'scannerserver' to an 'slib_entry' structure) */
    scannerserver *scanner;	/*:= !NULL */





    /*** TBD / WORK IN PROGRESS / TRYING THINGS OUT ***/
    /* token table */
    /* TBD */
    void *token_table;

    /* lex() callback */
    /* these are callbacks for the callbacks */
    int    (*lex)   (void *,void *, Hsp *); /* void usu YYSTYPE, YYLTYPE included from bison types */

    /* meta data */
    void *scandata;		/* custom data from scan */



    /************************* RETIRE THESE ***************************/
    
    /* scanner library context reference */ /* RETIRED -- moving to Axpa */
    scanlib *slib_ref;		/*:= !NULL */
    /* data source reference */
    void *data;			/* this is a parse packet *//* DEPRECATE this */
    /* RQL lookup scanner */
    scannerserver *lookupss;	/* THIS WILL BE RETIRED... */
    /* RQL packet */
    Ppak *rqpak;		/* THIS WILL BE RETIRED IN PLACE OF target */
    /* CALLBACKS */
    // void   (*put)   (void *, char *, char*, long int, long int, long int, long int);
    void   (*putob) (Hsp *, int, off_t, char *);
    Htok_t (*gettok)(Hsp *);
    FILE  *(*fopen) (Ppak *, char *);
    /******************************************************************/

}Hsp;





#endif /*!_HIENA_SCANNER_PAYLOAD_H_*/
