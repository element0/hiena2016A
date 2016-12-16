/** @file hiena_core.c

  hiena parses a layer of hierarchy in a domain cell using a scannerlib.

  --- 

  NOTE:
    
  include this file last in the include chain of file "hienafs.c"
  presently this file wraps the older "hsp" implementation.
  do not refer to 'hsp' anywhere outside of "hiena_core".
 
 */

//#include "hsp.c"	/*!< only include this here */
#include <hiena/server_module.h>
#include <hiena/scanner_module.h>  /* defines scanner payload type */

/*
int hiena_init () {
    hsp_init_globals();
}
*/

int hiena ( struct hiena_domaincell * ds, struct hiena_scannerlib * sl ) {
    if( ds == NULL || sl == NULL ) return 0;

    // open domaincell file handle
    struct hiena_server_file_handle * fh = hiena_open( &ds->addr, 0, "r", NULL );

    /* init scanner frame */
    Hsframe yyextra;
    memset( &yyextra, 0, sizeof(Hsframe));
    yyextra->op = ds->addr->scanlib->cb;
    yyextra->fh = fh;
    yyextra->dcel = ds;

    /* declare lexer object */
    yyscan_t lexer;

    // for each in scannerlib
    scannerserver * curscanner = scanlib->newest;
    while(curscanner != NULL) {
	// rewind fh
	hiena_seek( fh, 0, SEEK_SET );
	// init lexer
        curscanner->op->yylex_init_extra( &yyextra, &lexer );
	/* run parser */
	do{
	    curscanner->op->yyparse( lexer, &yyextra );
	}while(0);

	/* cleanup lexer */
	curscanner->op->yylex_destroy( lexer );
    }

    // close domaincell_fh
    hiena_close( fh );

    return 1;
}

/* end file hiena_core.c */
