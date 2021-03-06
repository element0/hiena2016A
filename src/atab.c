#include <stdio.h>
#include "ppak.h"
#include "hsp.h"
#include "atab.h"



typedef struct hiena_parse_packet Atab;
typedef struct ppak_list_element row_t;
typedef struct ppak_list_element col_t;


void atab_cleanup ( Atab *a ) {
    ppak_cleanup(a);
}

void atab_refresh ( Atab *a ) {
    ppak_refresh(a, NULL, NULL);
}

row_t *get_next_row(Atab *cur_tab, row_t *cur_row) {
    if(cur_tab == NULL) return NULL;

    row_t *result;

    if(cur_row == NULL) {
	result = ppak_get_first_child(cur_tab);
	return result;
    }
    if(list_verify(cur_row)) {
	row_t *result = list_get_next(cur_row);
	return result;
    }else{
	return NULL;
    }
}
col_t *get_next_col(Ppak *rowpak, col_t *cur_col) {
    if(rowpak == NULL) return NULL;

    return ppak_get_next_prop ( rowpak, cur_col );
}

static int row_match(row_t *row, const char *expr_lval, const char *expr_rval, int op) {
    /* == TRUE | FALSE */
    if(row == NULL || expr_lval == NULL || expr_rval == NULL || op == 0) return -1;

    /* iterate through "columns" AKA keys */
    Ppak *rowpak = (Ppak *) list_get_ob(row);
    Ppak *colpak = NULL;

    col_t *cur_col = NULL;
    while((cur_col = get_next_col( rowpak, cur_col )) != NULL) {

	colpak = (Ppak *) list_get_ob(cur_col);

	if(strncasecmp(hpat_str_ptr(colpak->key.name),
		       expr_lval,
		       colpak->key.name->len)) {

	    /* if colpak->val matches expr_rval by op */
	    switch(op) {
		case '~':
		    ;
		case '=':
		    if(strncmp(hpat_str_ptr(colpak->val.buf),
			       expr_rval,
			       colpak->val.buf->len)) {
			return 1;
		    }
		    ;
		case '!':
		    ;
		case '<':
		    ;
		case '>':
		    ;
	    }
	}


    }



    return -1;
}

static row_t *new_row( row_t *garbage ) {
    return new_list( garbage );
}


static int col_match(col_t *col, char *col_expr) {
    return -1;
}
static void add_row(Atab *dest_atab, row_t *src_row) {
    return;
}
static void add_col(row_t *dst_row, col_t *src_col) {
    return;
}
static int verify_cols(row_t *targ_row) {
    return -1;
}
static int verify_table(Atab *table) {
    return -1;
}
static void temp_row_cleanup(row_t *temp_row) {
    return;
}



Dpak *atab_simple_select (Dpak *targ, Hsp *h) {
    if(h == NULL || h->first_result_col == NULL) return NULL;

    Dpak  *res_tab  = new_ppak( NULL );/* TBD: where does this get cleaned up? */
    Dpak  *row_pak  = NULL;
    row_t *temp_row = NULL;
    row_t *cur_row  = NULL;
    col_t *cur_col  = NULL;

    /* LOCK ATAB */
    /* tbd */

    /* REFRESH TARGET TABLE */
    atab_refresh( targ );

    /* ... iterate through rows ... */
    while((cur_row = get_next_row(targ, cur_row)) != NULL) {

	if(row_match(cur_row, h->sql_expr_lval, h->sql_expr_rval, h->sql_expr_op)) {

	    row_pak = list_get_ob(cur_row);

	    temp_row = new_row( NULL );	/* TBD garbage */

	    /* ... iterate through columns ... */
	    while((cur_col = get_next_col(row_pak, cur_col)) != NULL) {

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
	return res_tab;
    } else {
	atab_cleanup(res_tab);
	return NULL;
    }
}


                                                                                                                                                                                