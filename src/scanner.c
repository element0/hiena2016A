/*_HIENA_SS_C_*/
/* only include this from sslib.c */

#include <stdio.h>


typedef void * yyscan_t; /* as per flex instructions */

typedef struct hiena_scannerserver_ops
{
    /* THESE MAY BE DEPRECATED VERY SOON */
    /* in favor of FSM */

    /* WORK IN PROGRESS */
    /* these are the functions provided by the scannerserver .so file */
    int    (*parse)           (void * hsp);
    char **(*show_allowed_child_scanners)();
    char  *(*get_tok_name)    (int tokid);

    int    (*yyparse)         (yyscan_t *, void * hsp);
    int    (*yylex_init_extra)(void * hsp, yyscan_t *);
    int    (*yylex_destroy)   (yyscan_t *);
    void   (*yyset_in)        (FILE *, yyscan_t *);
    FILE  *(*yyget_in)        (yyscan_t *);


}Hscannerops_t;

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
    char          * name;
    void          * dl;	        // for dlopen()
    void          * slib;
    void          * servers;	/* these are *intended* immutable - host relative, global */
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




scannerserver *hiena_scanner_create() {
    size_t size         = sizeof(scannerserver);
    scannerserver *scanner = malloc(size);
    if(scanner == NULL)
    {
	//fprintf(stderr, "hiena_scanner_create: failed to allocate memory for scannerserver, abort routine.\n");
	return NULL;
    }
    memset(scanner, 0, size);

    scanner->op = malloc(sizeof(Hscannerops_t));
    if(scanner->op == NULL) {
	free(scanner);
	return NULL;
    }

    return scanner;
}

void hiena_scanner_cleanup(scannerserver *scanner) {
    if(scanner==NULL){
	//fprintf(stderr,"hiena:scanner_system:trying to cleanup NULL scanner\n");
	return;
    }
    if(scanner->dl!=NULL){
#if HAVE_LTDL_H
	lt_dlclose( scanner->dl );
#else
	//char *dlerr=NULL;//dlerror();
	if(dlclose(scanner->dl)!=0){
	    //fprintf(stderr,"hiena_scanner_cleanup dlclose err :%s\n",dlerr);	
	}
#endif
    }
    free(scanner->name);
    free(scanner->op);
    free(scanner);
}


/******* TODO ****************/
/* use struct hiena_scannerlib_entry instead of scannerserver */
/* using slib_entry structure will implicity include a link to slib, so
   we can thenceforth remove 'slib' from these arguments. */
/*
void do_scanner (scannerserver *s, Ppak *p, scanlib *slib) {
    if(s == NULL || p == NULL) {
	//fprintf(stderr, "do_scanner: NULL scanner or Ppak, abort.\n");
	return;
    }
    Hsp *h = hsp_init(p);
    hsp_set_scanner_ptr(h,s);

    if(h == NULL)
    {
	fprintf(stderr, "do_scanner: can't init Hsp, abort.\n");
	return;
    }

    hsp_execute(h);


    hsp_cleanup(h);
    return;
}

void do_filter (scannerserver *filter, Ppak *p) {
    do_scanner (filter, p, NULL);
}
*/
/*
void do_scanners (Ppak *p, scanlib *slib) {
*/
    /*
       for each scanner in slib
       do
           scanner( p )
       done
       */
/*
    scannerserver *s = slib_get_next_scanner(slib, NULL);
    while(s != NULL) {
	// TODO: multi-threaded version
	do_scanner(s, p, slib);
	s = slib_get_next_scanner(slib, s);
    }

    return;
}
*/

/*--------*/

