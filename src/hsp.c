#include <string.h>
#include <errno.h>
#include <stdarg.h>
/* #include "rql.h" */		/* RETIRING */
#include "hsp.h"
#include "accesspath.h"

//#include "rql.c"

/*== GLOBAL VARS: ==*/

/* requires calling a one time init routine and a one time clenaup. */
Hspops *hsp_ops_global = NULL;


/* forward decl */
int hsp_ops_cleanup(Hspops *);
Hspops *hsp_ops_init();


Hspops *hsp_init_globals () {
    return hsp_ops_global = hsp_ops_init();
}

void hsp_globals_cleanup () {
    hsp_ops_cleanup(hsp_ops_global);
}

/*--------*/

/*== HOUSEKEEPING FUNCTIONS: ==*/
/*
static Hpat *validate_rqfunc_args (Hsp *h, const char *s)
{
    if(h != NULL && s != NULL)
    {
	Hpat *pat = new_hpat_from_string(s);
	if(pat==NULL)
	{
	    //fprintf(stderr, "sshsp:validate_rqfunc_args: hpat is NULL, abort routine.\n");
	    return NULL;
	}
	if(h->rq == NULL)
	{
	    h->rq = new_rq();
	    if(h->rq == NULL)
	    {
		//fprintf(stderr, "sshsp:validate_rqfunc_args: can't create h->rq, abort routine.\n");
		hpat_cleanup(pat);
		return NULL;
	    }
	}
	return pat;
    }else{
	//fprintf(stderr, "sshsp:validate_rqfunc_args: h or s are NULL, abort routine.\n");
	return NULL;
    }
}
*/

/*--------*/

/*== SQL INTERNAL IMPLEMENTATION FUNCTIONS: ==*/
/* previously located in "atab.c" */

#include "sqlinterp/rql.h"
#include "sqlinterp/rql.tab.h"


int hsp_set_sql_selection ( Hsp *h, int tabid ) { /* 1 | ERROR */
    if(h == NULL) return -1;

    if(h->selection == NULL) {
	h->selection = axpa_get_domain(h->axpa);
	if(h->selection == NULL) {
	    fprintf(stderr, "hsp_set_sql_selction: axpa has NULL selection. abort.\n");
	    return -1;
	}
    }

    if(tabid == TAB_SEL_CHILD) {
	/* do nothing, keep h->selection */
    }else 
    if(tabid == TAB_SEL_PROP) {
	/* tricky situ, we need to transpose h->selection's prop list
	   to a new dpak's child list */
	Dpak *prop_sel = new_ppak(h->selection);
	if(prop_sel != NULL) {
	    /* tbd derive(propsel, h->selection); */
	    /*   deriving prop_sel from selection
		 should involve retaining the prop list of selection */
	    prop_sel->first_child = h->selection->first_prop;

	    h->selection = prop_sel;
	    /* if props are empty this will be empty */
	    return 1
	}else{
	    return -2;
	}
    }
    h->selection = NULL;
    return 1;
}

static int row_match(list_t *row, const char *expr_lval, const char *expr_rval, int op) {
    /* == TRUE | FALSE */
    if(row == NULL || expr_lval == NULL || expr_rval == NULL || op == 0) return -1;

    /* iterate through "columns" AKA keys */
    Ppak *rowpak = (Ppak *) list_get_ob(row);
    Ppak *colpak = NULL;

    list_t *cur_col = NULL;
    while((cur_col = ppak_get_next_col( rowpak, cur_col )) != NULL) {

	colpak = (Ppak *) list_get_ob(cur_col);

	if(strncasecmp(hpat_str_ptr(colpak->key.name),
		       expr_lval,
		       colpak->key.name->len)) {

	    /* if colpak->val matches expr_rval by op */
	    switch(op) {
		case '~':
		    break;
		case '=':
		    if(strncmp(hpat_str_ptr(colpak->val.buf),
			       expr_rval,
			       colpak->val.buf->len)) {
			return 1;
		    }
		    break;
		case '!':
		    break;
		case '<':
		    break;
		case '>':
		    break;
	    }
	}
    }
    return 0;
}


/*--------*/


/*== CALLBACK FUNCTIONS: used in payload callback operations object == */

/*   **** MAIN INTERFACE FOR SCANNERSERVER PROGRAMMERS ****    */

/*   these functions provide a wrapper for functions defined elsewhere in the
 *   hiena project.  the intent is to minimize the interface for programmers
 *   who are writing scannerserver code.
 *
 *   These functions may take DIFFERENT arguments than the functions they
 *   represent.  This is NOT A DISCREPENCY. 
 */
void hsp_clear_nova (Hsp *hsp) {
    if(hsp->first_arg != NULL) {
	nova_t *cur_arg = hsp->first_arg;
	nova_t *nextnova;
	while((nextnova = cur_arg->next) != NULL) {
	    free(cur_arg);
	    cur_arg = nextnova;
	}
	free(cur_arg);
	hsp->first_arg = NULL;
    }
}

void hsp_clear_sql_rescol (Hsp *h) {
    if(h->first_result_col != NULL) {
	nova_t *cur_rescol = h->first_result_col;
	nova_t *next_rescol;
	while((next_rescol = cur_rescol->next) != NULL) {
	    free(cur_rescol);
	    cur_rescol = next_rescol;
	}
	free(cur_rescol);
	h->first_result_col = NULL;
    }
}

int hsp_set_nova (Hsp *h, void *var) {
    if(h==NULL || var==NULL)	return -1;

    nova_t *nova = malloc(sizeof(nova_t));
    if(nova == NULL)
	return -1;
    memset(nova,0,sizeof(nova_t));

    nova->var = var;

    if(h->first_arg == NULL) {
	h->first_arg = nova;
    }else{
	h->last_arg->next = nova;
    }
    h->last_arg = nova;
    return 1;
}



/* rescol(VAR) */
int hsp_sql_set_rescol(Hsp *h, void *var) {
    if(h==NULL || var==NULL)	return -1;

    nova_t *nova = malloc(sizeof(nova_t));
    if(nova == NULL)
	return -1;
    memset(nova,0,sizeof(nova_t));

    nova->var = var;

    if(h->first_result_col == NULL) {
	h->first_result_col = nova;
    }else{
	h->last_result_col->next = nova;
    }
    h->last_result_col = nova;
    return 1;
}
/* ANALYTICAL TABLE ABSTRACT SUPPORT FUNCTIONS */
static int col_match(list_t *col, char *col_expr) {
    return -1;
}
static void add_row(Dpak *dest_atab, list_t *src_row) {
    return;
}
static void add_col(list_t *dst_row, list_t *src_col) {
    return;
}
static int verify_cols(list_t *targ_row) {
    return -1;
}
static int verify_table(Dpak *table) {
    return -1;
}
static void temp_row_cleanup(list_t *temp_row) {
    return;
}


/* do_select() */
int hsp_sql_select_core ( Hsp *h ) { /* TRUE | FALSE */
    if(h == NULL || h->first_result_col == NULL
	         || h->selection == NULL) return 0;

    /* WORK IN PROGRESS: REMOVING "ATAB" ABSTRACTION */
    Dpak   *res_tab  = new_ppak( NULL );/* TBD: where does this get cleaned up? */
    if(res_tab == NULL) return 0;

    Dpak   *row_pak  = NULL;
    list_t *temp_row = NULL;
    list_t *cur_row  = NULL;
    list_t *cur_col  = NULL;

    /* TBD: LOCK "h->selection" */

    /*--- REFRESH DPAK ---*/
    ppak_refresh( h->selection, h->axpa, h );

    /*--- iterate through "rows" in h->selection ---*/
    while((cur_row = get_next_row(h->selection, cur_row)) != NULL) {

	if(row_match(cur_row, h->sql_expr_lval, h->sql_expr_rval, h->sql_expr_op)) {
	
	    row_pak = list_get_ob(cur_row);
	    temp_row = new_list( NULL );	/* TBD garbage */
	    if(temp_row == NULL)
		return 0;
	    /* ... iterate through columns ... */
	    while((cur_col = ppak_get_next_col(row_pak, cur_col)) != NULL) {
		if(col_match(cur_col, h->sql_col_expr)) {
		    add_col(temp_row, cur_col);
		}
	    }
	    /* build result table */
	    if(verify_cols(temp_row)) {
		add_row(res_tab, temp_row);
	    } else {
		temp_row_cleanup(temp_row);
	    } 
	}
    }
    /* result table is ready */
    /* and quite possibly empty... */

    /* UNLOCK ATAB */

    if(verify_table(res_tab)) {
	/* SET HSP->SELECTION = res_tab */
	return 1;
    } else {
	ppak_cleanup(res_tab);
	return 0;
    }
}

/* from(TARG) */
int hsp_sql_set_selection ( Hsp *h, int tabid ) {
    if(h == NULL) return -1;

    if(h->selection == NULL) {
	h->selection = axpa_get_domain(h->axpa);
	if(h->selection == NULL) {
	    fprintf(stderr, "hsp_set_sql_selction: axpa has NULL selection. abort.\n");
	    return -1;
	}
    }
    if(tabid == TAB_SEL_CHILD) {
	/* do nothing, keep h->selection */
    }else 
    if(tabid == TAB_SEL_PROP) {
	/* tricky situ, we need to transpose h->selection's prop list
	   to a new dpak's child list */
	Dpak *prop_sel = new_ppak(h->selection);
	if(prop_sel != NULL) {
	    /* tbd derive(propsel, h->selection); */
	    /*   deriving prop_sel from selection
		 should involve retaining the prop list of selection */
	    prop_sel->first_child = h->selection->first_prop;

	    h->selection = prop_sel;
	    /* if props are empty this will be empty */
	    return 1;
	}else{
	    return -2;
	}
    }
    return -3;
}




int hsp_sql_getchar(Hsp *h) {
    if(h == NULL) {
	return EOF;
    }
    /* h must be initialized by a call to hsp_sql first */
    
    int res = '\0';

    if(h->cur_arg_strp != NULL) {
	if(h->cur_arg_strp[0] == '\0') {
	    h->cur_arg_strp = NULL;
	    if(h->cur_arg != NULL)
		h->cur_arg = h->cur_arg->next;
	}else{
	    res = h->cur_arg_strp[0];
	    h->cur_arg_strp++;
	    /* printf("%c",res); */
	    return res;
	}
    }

    if(h->strp == NULL) {
	/* printf("\n"); */
	return EOF;
    }
    if(h->strp[0] == '\0') {
	h->strp = NULL;
	/* printf("\n"); */
	return EOF;
    }
    if(h->strp[0] == '%') {
	h->strp++;
	switch(h->strp[0]){
	    case 's':
		h->strp++;
		if(h->cur_arg == NULL) {
		    break;
		}
		if(h->cur_arg->var == NULL) {
		    h->cur_arg = h->cur_arg->next;
		    break;
		}
		if(h->cur_arg_strp == NULL) {
		    h->cur_arg_strp = (char *)h->cur_arg->var;
		}
		if(h->cur_arg_strp[0] == '\0') {
		    h->cur_arg_strp = NULL;
		    h->cur_arg = h->cur_arg->next;
		    break;
		}else{
		    res = h->cur_arg_strp[0];
		    h->cur_arg_strp++;
		    /* printf("%c",res); */
		    return res;
		    break;
		}
	}
    }
    res = h->strp[0];
    /* printf("%c",res); */
    h->strp++;
    return res;
}

int hsp_sql(Hsp *h, const char *sqlstr) {
    if(h == NULL || sqlstr == NULL) return 0;

    /*--- set sql request's printf-style string ---*/
    h->strp = (char *)sqlstr;
    /*--- prep sql request's variadic arguments ---*/
    h->cur_arg = h->first_arg;		/* args prepared by calling scanner */

    /*--- run "rql" sql scanner unit ---*/
    yyscan_t scanner;
    /*    defined in "src/rql-bison/"   */
    rqllex_init_extra( h, &scanner );
    rqlset_in(NULL, scanner);		/* scanner reads custom YY_INPUT */
    rqlparse(scanner, h);
    rqllex_destroy(scanner);
    /*----------------------------*/

    /*--- cleanup result expr list ---*/
    hsp_clear_sql_rescol(h);
    /*--- cleanup sql request's variadic arguments ---*/
    hsp_clear_nova(h);
    return 1;
}

char *hsp_getenv(const char *envarname) {
    /* TODO: use axpa to retrieve envar value rather than stdlib */

    /**** PLACEHOLDER: get_envar: use stdlib's getenv() ****/
    return getenv(envarname);
}

/*
void set_rqtarg (Hsp *h, const char *s)
{
    Hpat *pat = validate_rqfunc_args(h,s);
    if(pat==NULL)
    {
	//fprintf(stderr, "sshsp:set_rqtarg: pat is NULL, abort routine.\n");
	return;
    }
    //set_targ(h->rq, pat);
    return;
}

void set_rqdest (Hsp *h, const char *s)
{
    Hpat *pat = validate_rqfunc_args(h,s);
    if(pat==NULL)
    {
	//fprintf(stderr, "sshsp:set_rqdest: pat is NULL, abort routine.\n");
	return;
    }
    set_dest(h->rq, pat);
    return;
}

void set_rqsrc (Hsp *h, Ppak *s)
{
    if(h == NULL || s == NULL)
    {
	//fprintf(stderr, "sshsp:setrqsrc: h or s is NULL, abort routine.\n");
	return;
    }
    set_src(h->rq, s);
    return;
}

void set_rqmap (Hsp *h, int i)
{
    //fprintf(stderr, "set_rqmap: Hsp %p, int %d\n",h,i);
    return;
    if(h == NULL)
    {
	//fprintf(stderr, "sshsp:setrqmap: h is NULL, abort routine.\n");
	return;
    }
    if(h->rq == NULL)
    {
	h->rq = new_rq();
	if(h->rq == NULL)
	{
	    //fprintf(stderr, "sshsp:setrqmap: can't init h->rq, abort routine.\n");
	    return;
	}
    }
    set_map(h->rq, i);
    return;
}

void set_rqop (Hsp *h, int i)
{
    if(h == NULL)
    {
	//fprintf(stderr, "sshsp:set_rqop: h is NULL, abort routine.\n");
	return;
    }
    if(h->rq == NULL)
    {
	h->rq = new_rq();
	if(h->rq == NULL)
	{
	    //fprintf(stderr, "sshsp:set_rqop: can't init h->rq, abort routine.\n");
	    return;
	}
    }
    set_op(h->rq, i);
    return;
}

void hsp_rq_init (Hsp *h, Ppak *p){
    if(h == NULL)
    {
	//fprintf(stderr, "set_rqres: h is NULL, abort.\n");
	return;
    }
    if(h->rq == NULL)
    {
	h->rq = new_rq();
	if(h->rq == NULL)
	{
	    //fprintf(stderr, "set_rqres: can't init h->rq, abort.\n");
	    return;
	}
    }
    set_res(h->rq, p);	
    // rql_set_hsp(h->rq, h);
}
*/

Htok_t hsp_gettok ( Hsp *h ) {
    return 0;
}

size_t hsp_dsread ( void *ptr, size_t count, size_t size, Hsp *h ) {
    if ( h == NULL )  return 0;

    return dpak_dsread( ptr, count, size, h->ds );
}
int hsp_ferror ( Hsp *h ) {
    return 0;
}
void hsp_clearerr ( Hsp *h ) {
}






/*== OBJECT IMPLEMENTATION: hiena scanner payload callback operations == */

Hspops *hsp_ops_create() {
    Hspops *op = malloc(sizeof(Hspops));
    if(op == NULL)
	return NULL;

    memset(op,0,sizeof(Hspops));

    return op;
}

Hspops *hsp_ops_init() {
    Hspops *op = hsp_ops_create();
    if(op == NULL)
	return NULL;

    /* SCANNING FUNCTIONS */
    op->gettok     = hsp_gettok;
    op->dsread     = hsp_dsread;
    op->ferror     = hsp_ferror;
    op->clearerror = hsp_clearerror;

    /* MAPPING FUNCTIONS */
    op->make_ob    = ppak_make_ob;
    op->set_bounds = ppak_set_bounds;
    op->set_buf    = ppak_set_map_buf;

    op->add_prop   = ppak_add_prop_hsp;
    op->add_child  = ppak_add_child_hsp;

    op->nova	   = hsp_set_nova;
    op->sql        = hsp_sql;
    op->sql_getchar = hsp_sql_getchar;


    /* CONTEXT FUNCTIONS */
    op->getenv = hsp_getenv;

    return op;
}

int hsp_ops_cleanup(Hspops *hops)
{
    free(hops);
    return 0;
}


/*== OBJECT IMPLEMENTATION: hiena scanner payload == */

Hsp *
hiena_scanner_payload_create() {
    Hsp *hsp = malloc(sizeof(struct hiena_scanner_payload));
    if(hsp == NULL)
	return NULL;
    memset(hsp,0,sizeof(struct hiena_scanner_payload));

    return hsp;
}
expr_t *new_expr() {
    expr_t *e = malloc(sizeof(*e));
    if(e == NULL)
	return NULL;
    memset(e, 0, sizeof(*e));
    return e;
}

void expr_cleanup(expr_t *e) {
    free(e);
}


int rql_push_expr ( Hsp *h, Htok_t type, void *val) {
    if(h == NULL) return -1;

    expr_t *e = new_expr();
    if(e == NULL) return -1;

    e->type = type;
    e->val  = val;

    if(h->first_expr == NULL) {
	h->first_expr = e;
	h->last_expr  = e;
    }else{
	h->last_expr = e;
    }

    return 1;
}

void expr_cleanup_stack ( expr_t *e ) {
    expr_t *ex = e;
    expr_t *nex = NULL;
    while(ex != NULL) {
	nex = ex->next;
	expr_cleanup(ex);
	ex = nex;
    }
}

void
hsp_cleanup(Hsp *hsp) {
    if(hsp == NULL) return;

    /* garbage collector */
    // hiena_parse_packet_take_out_the_garbage(hsp->parseroot);

    /* RQL lookup workspace */
    /*
    if(hsp->rq != NULL)
    {
	rq_cleanup(hsp->rq);
    }
    */

    hsp_clear_nova(hsp);
    if(hsp->first_expr != NULL)
    {
	expr_cleanup_stack(hsp->first_expr);
    }


    if(hsp->lexer != NULL)
    {
	hsp->scanner->op->yylex_destroy(hsp->lexer);
    }

    /* hsp->op is global,
       only free with "hsp_globals_cleanup" at program end. */

    if(hsp->fp != NULL)
    {
	fclose(hsp->fp);
	hsp->fp = NULL;
    }

    /* hsp->target  -- reference pointer, do not cleanup. */
    free(hsp);
}
Hsp *new_hsp() {
    return hiena_scanner_payload_create();
}

void hiena_scanner_payload_cleanup(Hsp *hsp) {
    hsp_cleanup(hsp);
}

static Hsp *verify_or_kill_hsp(Hsp *h) {
    /* make sure hsp values follow proper constraints 
       as notated in the comments of "hsp.h"
       IF NOT cleanup 'h' and return NULL.
     */
    if(h == NULL) {
	//fprintf(stderr, "verify_or_kill_hsp: hsp NULL, abort.\n");
	fflush(stderr);
	return NULL;
    }
    if(
	    h->target     == NULL  
	 //|| h->axpa        == NULL	/* required? */
	 || h->op          == NULL
	 //|| h->rq          == NULL	/* RETIRE */
	 || h->scanner == NULL
	 )
    {
	fprintf(stderr,"verify_or_kill_hsp: hsp fails constraint check, abort\n");
	fflush(stderr);
	//hsp_cleanup(h);
	return NULL;
    }
    return h;
}

Hsp *hsp_init ( Dpak *target ) {

    /* this inits everything but not 'ax' nor 'scanner__ref'
       nor 'fp' nor 'lexer' */

    Hsp *hsp = new_hsp();
    if ( hsp == NULL ) {
	//fprintf(stderr, "hsp_init: can't create hsp, abort routine.\n");
	//fflush(stderr);
	return NULL;
    }

    /* parse-target source-packet */ /*<---- MAKE THIS GARBAGE COLLECTOR */
    /* this will be the target that scanners are run against */
    hsp->target = target;
    
    /* INIT CALLBACKS */
    if(hsp_ops_global == NULL) 
	hsp_init_globals();		/* a courtesy if you didn't init this */	

    hsp->op = hsp_ops_global;
    if(hsp->op == NULL) {
	fprintf(stderr, "hsp_init: can't init hsp->op, abort.\n");
	return NULL;
    }

    /* ...be sure to call hsp_globals_cleanup on program end. */

    return hsp;
}

int hsp_set_target ( Hsp *h, Dpak *p ) { /* TRUE | FALSE */
    if ( h == NULL ) return 0;
    h->target = p;
    return 1;
}


int
hiena_scanner_session_rewind(Hsp *scanner_session) {
    scanner_session->cur_location = 0;

    return 0;
}

Axpa *hsp_get_axpa (Hsp *h) {
    if(h == NULL)
	return NULL;

    return h->axpa;
}


int hsp_set_axpa (Hsp *h, Axpa *a) { /* == TRUE | FALSE */
    if ( h == NULL ) return 0;
    h->axpa = a;
    return 1;
}


int hsp_set_scanlib (Hsp *h, scanlib *slib)
{
    if(h == NULL)
    {
	return -1;
    }
    h->slib_ref = slib;
    return 0;
}


int hsp_set_scanner_ptr ( Hsp *h, scannerserver *s ) { /* TRUE | FALSE */
    if( h == NULL ) return 0;
    h->scanner = s;

    return 1;
}

/* hsp_init_scanner
   support function for "hsp_set_scanner_name"
   also called by dswrite() functions
*/

int hsp_init_scanner (Hsp *hsp, scannerserver *s) { /* TRUE | FALSE */
    if( hsp == NULL ) return 0;

    hsp->scanner = s;
    /* NO CLEANUP REQUIRED:
       hsp->scanner is just a reference
       we can replace the pointer without cleanup
       if s is NULL this resets the pointer
     */

    /* SETTING NULL SCANNER IS OK -- BUT POINTLESS */
    if(s == NULL) return 1;

    if(s->op == NULL) return 0;
    /* then we can't initialize flex-bison session */

    int err;
    if ( hsp->lexer != NULL ) {
    /* CLEANUP REQUIRED:
       however if hsp->lexer is NOT NULL
       we must clean it up before init'ing a new lexer.
     */
	if(s->op->yylex_destroy != NULL)
	{
	    err = s->op->yylex_destroy(hsp->lexer);
	    if(err != 0)
	    {
		//fprintf(stderr, "hiena:hsp.hsp_set_scanner: trouble destroying previous lexer, abort routine.\n");
		return 0;
	    }
	}
    }

    if(s->op->yylex_init_extra != NULL)
    {
	err = s->op->yylex_init_extra( hsp, &hsp->lexer );
	if(err != 0)
	{
	    perror("hiena:hsp.hsp_init_scanner");
	    //fprintf(stderr, "hiena:hsp.hsp_init_scanner: can't initialize hsp, abort routine.\n");
	    return 0;
	}
    }

    return 1;
}


/* hsp_set_scanner_name
   takes an hsp and a string 
rationale: the scannerserver should come from out of the 
   library located in the access path.  the hsp should have
   an access path.  an hsp is intended to be limited
   to those resources which are provided by the access path.
   therefor, we should request a scanner from the access path
   rather than pass a scanner in through the hsp.
   The request for a scanner must be recognized by the scannerlib.
   First version uses a scanner name.
TBD:
   maybe we can use a global id rather than a name string.
*/

int hsp_set_scanner_name (Hsp *h, const char *sname ) { /* TRUE | FALSE */
    if(h == NULL || sname == NULL)
	return 0;
    
    Axpa *axpa    = hsp_get_axpa(h);
    if(axpa == NULL)
	return 0;
    
    scanlib *slib = axpa_get_scanlib(axpa);
    if(slib == NULL)
	return 0;
    
    scannerserver *ss = slib_get_scanner(slib, sname);
    if(ss == NULL)
	return 0;
    
    hsp_init_scanner(h, ss);	// sets scanner ptr and inits lexer
    return 1;
}


Hsp *hsp_init_src_scanner_slib(Ppak *target, scannerserver *s, scanlib *slib) {
    
    Hsp *h = new_hsp();
    if(h == NULL)
    {
	//fprintf(stderr, "hsp_init_src_scanner_slib: can't create hsp, abort.\n");
	return NULL;
    }

    h->op = hsp_ops_init();
    if(h->op == NULL) {
	hsp_cleanup(h);
	//fprintf(stderr, "hsp_init_src_scanner_slib: can't init ops, abort.\n");
	return NULL;
    }
    // h->rq = new_rq();
    // rql_set_hsp(h->rq,h);

    h->target = target;
    h->parseroot = target; // new_ppak(NULL);

    if(target != NULL)
    {
	h->fp = ppak_fopen(target, "r");
    }
    /* TBD check error codes... */
    /* how necessary are error codes in this situation, really? */
    hsp_init_scanner(h, s);
    hsp_set_scanlib(h, slib);

    return verify_or_kill_hsp(h);
}

int hsp_open_stream (Hsp *hsp) { /* TRUE | FALSE */
    if ( hsp == NULL ) return 0;

    scannerserver *s = hsp->scanner;
    if ( s == NULL ) return 0;

    if ( hsp->lexer == NULL ) return 0;

    if ( hsp->ds == NULL ) {
	if ( hsp->target != NULL ) {

	    hsp->ds = dpak_dsopen( hsp->target, "r" ); 
	    if ( hsp->ds == NULL ) {
		fprintf(stderr, "hsp_open_stream: dpak_dsopen returned NULL.\n");
		return 0;
	    }
	}else{
	    //fprintf(stderr, "hiena:hsp.hsp_set_scanner: FILE fp and source are NULL, abort routine.\n");
	    return 0;
	}
    }
    return 1;

    /* version 2 uses dpak stream, accessed through YY_INPUT, so yyset_in can be ignored */
    /*
    //fprintf(stdin, "setting yyset_in...\n");
    if ( s->op->yyset_in != NULL && s->op->yyget_in != NULL ) {

	s->op->yyset_in( hsp->fp, hsp->lexer );
	FILE *fpcheck = s->op->yyget_in(hsp->lexer);
	if(hsp->fp != fpcheck) {
	    //fprintf(stderr, "hsp_set_scanner: setting lexer input stream failed, abort routine.\n");
	    return -6;
	}
    }else{
	//fprintf(stderr,"hsp_set_scanner:yyset_in or yyget_in are NULL\n");
	return -1;
    }
    */
}

void hsp_close_stream ( Hsp * h ) {
    if ( h != NULL && h->ds != NULL ) {
	dpak_dsclose( h->ds );
	h->ds = NULL;
    }
}

Axpa *hsp_execute (Hsp *h) { /* == TRUE | ERROR */

    /* okay, so its expected
       that you have a Hsp setup by the time you get here.
       let's check if you've done your job...
       */

    if ( verify_or_kill_hsp( h ) == NULL ) {
	/* I don't think this actually kills HSP - or needs to. */
	fprintf(stderr, "hsp_execute: hsp fails verification, abort.\n");
	return NULL;
    }
    /* good job setting up the hsp */

    scannerserver *s = h->scanner;

    if ( (s->op != NULL) && (s->op->yyparse != NULL) && (h->lexer != NULL) )
    {
	/* OPEN SOURCE FILE STREAM */
	if ( ! hsp_open_stream( h )) {
	    fprintf(stderr, "hsp_execute: hsp can't open stream, abort.\n");
	    return NULL;
	}

	/* RUN SCANNER */
	s->op->yyparse(h->lexer, h);
	/* calls hsp_dsread() from the lexer's YY_INPUT section */
	

	


	/* CLOSE FILE STREAM */
	hsp_close_stream(h);

    }else{
	//fprintf(stderr, "hsp_exeucte: can't run scanner %s...\n", s->name);
	//fprintf(stderr, "do_scanner: s->op == %lu\n", 
	//	(long unsigned int)s->op);
	//fprintf(stderr, "do_scanner: s->op->yyparse == %lu\n",
	//	(long unsigned int)s->op->yyparse);
	//fprintf(stderr, "do_scanner: h->lexer == %lu\n",
	//	(long unsigned int)h->lexer);
    }

    /* MAKE ACCESS PATH OF SELECTION */
    
    //fprintf(stderr, "return access path of rql selection.\n");
    fflush(stderr);

    return h->axpa;
}

