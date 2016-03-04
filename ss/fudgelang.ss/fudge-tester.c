#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "../../src/libhiena.c"		/* USE HIENA LIBRARY */
		/* defines wrap_dlsym() */

/* yyscan_t defined in "sshsp.h", included in libhiena */


int
main(int argc, char *argv[])
{
    /* DYLIB */
    dlerror();
    void *dl = dlopen("./fudge.so", RTLD_NOW);
    if (!dl)
    {
	fprintf(stderr, "fudge-tester: %s\n", dlerror());
	exit(EXIT_FAILURE);
    }
    
    int (*yyparse)(yyscan_t *, Hsp *);
    int (*yylex_init_extra)(Hsp *, yyscan_t *);
    int (*yylex_destroy)(yyscan_t *);


    dlerror();
    yyparse = (int (*)(yyscan_t *, Hsp *))wrap_dlsym(dl, "yyparse",NULL);
    if(yyparse == NULL)
    {
	goto dylib_cleanup;
    }
    dlerror();
    yylex_init_extra = (int (*)(Hsp *, yyscan_t *))wrap_dlsym(dl, "yylex_init_extra",NULL);
    if(yylex_init_extra == NULL)
    {
	goto dylib_cleanup;
    }
    dlerror();
    yylex_destroy = (int (*)(yyscan_t *))wrap_dlsym(dl, "yylex_destroy",NULL);
    if(yylex_destroy == NULL)
    {
	goto dylib_cleanup;
    }



    /* REGULAR STUFF */
    Hsp *h = hiena_scanner_payload_init(NULL); // usu' takes ppak as arg

    yyscan_t scanner;
    yylex_init_extra( h, &scanner );

    do{
	//set_targ("working_dir");
        //printf("set_targ(%s)\n",NULL);
    	yyparse(scanner, h);
    }while(0);

    yylex_destroy(scanner);
    hiena_scanner_payload_cleanup(h);

    /* DYLIB CLEANUP */
dylib_cleanup:
    dlclose(dl);


    return 0;
}
