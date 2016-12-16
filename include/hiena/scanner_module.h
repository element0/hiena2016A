/*_HIENA_SCANNER_PAYLOAD_H_*/
#ifndef _HIENA_SCANNER_PAYLOAD_H_
#define _HIENA_SCANNER_PAYLOAD_H_

#include <stdio.h>
#include <sys/types.h>	/* off_t */

typedef void * yyscan_t;

/** Hiena convenience macros for Bison & Flex grammars

   note: for these to work in both bison and flex grammars
   the bison grammar must use:
   	%parse-param {Hsp * yyextra}
   and flex grammar must use:
   	%option extra-type="Hsp *"
 */
#define hiena_new_mapcel(HIENA_TOKID_ARG) $$ = yyextra->op->new_mapcel( HIENA_TOKID_ARG, yyextra )
#define hiena_add_mapcel(HIENA_MAPCEL_ARG) yyextra->op->add_mapcel( $$, HIENA_MAPCEL_ARG, yyextra )
#define hiena_set_celstr(HIENA_MAPCEL_ARG, HIENA_VALUE_ARG) yyextra->op->set_val( HIENA_MAPCEL_ARG, HIENA_VALUE_ARG, strlen(HIENA_VALUE_ARG) )

#define hiena_add_child(HIENA_MAPCEL_ARG) yyextra->op->add_child( $$, HIENA_MAPCEL_ARG, yyextra );

#define hiena_verbose(HIENA_DEBUG_STRING) fprintf(stderr, "\n"; \
	fprintf(stderr, HIENA_DEBUGSTRING ":\t"); \
	fflush(stderr);


/**
   the hiena scanner payload callback operations
   for a scannerserver.
 */
typedef struct hiena_scanner_callbacks
{
    void *(*new_mapcel) (off_t pos, size_t len, Htok_t relative_element_id, Hsp * );
    // hsp must have following context: 'current fcel read location', 'previously mapped cel'
    int   (*add_mapcel) (void * dstcel, void * srccel, Hsp * );
    int   (*add_child)  (void *, void *, Hsp *);
    int   (*set_val)    ( void * dstcel, void * ptr, size_t len );
    Htok_t (*gettok)    (Hsp *);
}Hscb;


typedef struct hiena_scanner_frame
{
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

    /* file handle */
    void * fh;

    /* callbacks */
    Hscb *op;			/*:= !NULL */

    /* the domain cell */
    dcel * dcel;
}Hsframe;





#endif /*!_HIENA_SCANNER_PAYLOAD_H_*/
