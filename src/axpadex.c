#include "axpadex.h"
#include "string.h"

Axpadex *new_axpadex () {
    Axpadex *ad = malloc(sizeof(ad*));
    memset(ad, 0, sizeof(ad*));

    ad->next_axno = 1;		/* MUST initialize to 1, 0 is error code */
    db = ad->ind = kcdbnew();
    
    if (!kcdbopen(db, "+", KCOWRITER)) {	/* open as B+ tree */
	kcdbdel(db);
	return NULL;
    }

    return ad;
}

void axpadex_cleanup ( Axpadex *ad ) {
    if(ad == NULL) return;
    if(ad->ind != NULL) {
	db = ad->ind;
	kcdbclose(db);
	kcdbdel(db);
    }
    if(ad->axroot != NULL) {
	axpa_cleanup( ad->axroot );
    }
    free(ad);
}


axno_t axpadex_get_new_axno ( Axpadex *ad ) {
    if(ad == NULL) return 0;
    /* overflow logic -- make sure axno_t is UNSIGNED or rewrite this overflow logic */
    if(ad->next_axno == (axno_t)-1) {
	fprintf(stderr, "axpadex_get_new_axno: axno_t overflow detected. returning 0.\n");
	return 0;
    }
    return ad->next_axno++;
}


/**
 * Add an access path to the access path index, return its new access number.
 *
 */
axno_t axpadex_add_axpa ( Axpadex *ad, Axpa *axpa ) { /* >0 | 0 */
    if(ad == NULL || ad->ind == NULL) return 0;
    db = ad->ind;
    
    axno_t axno  = axpadex_get_new_axno( ad );
    if( axno == 0 ) {
	fprintf(stderr, "axpadex_add_axpa: axpadex_get_new_axno returned 0. returning 0.\n");
	return 0;
    }
    
    if(!kcdbadd(db, &axno, sizeof(axno), &axpa, sizeof(axpa))) {
	fprintf( stderr, "axpadex_add_axpa: kcdbadd failed.  (%s)  returning.\n", kcdbemsg(db));
	return 0;
    }
    if(!axpa_set_garbage( axpa, ad->axroot )) {
	fprintf( stderr, "axpadex_add_axpa: axpa_set_garbage failed. returning.\n");
	return 0;
    }

    return 1;
}

