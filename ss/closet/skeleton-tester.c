#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "../../src/hpat.c"
#include "../../src/atab.c"
#include "../../src/scanner_session.c"

int (*parse)(FILE *, Hsp *);


/* skeleton tester */


int
main(int argc, char *argv[])
{
    /* INITIALIZE INPUT */
    /* this buffer can contain whatever you like */
    /* (see "hpat.c" for methods) */
    Hpat *buf = new_hpat_from_argv(argc,argv);

    /* INITIALIZE SCANNER */
    FILE *fp  = hpat_fopen(buf, "r");
    Hsp  *hsp = hiena_scanner_payload_create();
    Atab *a   = hiena_atab_create();
    hsp->data = a;
    hsp->put  = atab_put;

    char *error;
    int   err;

    void *dl = dlopen("./skeleton.so", RTLD_NOW);
    if (!dl) {
	fprintf(stderr, "skeleton-tester: %s\n", dlerror());
	exit(EXIT_FAILURE);
    }
    dlerror();

    parse = (int (*)(FILE *, Hsp *)) dlsym(dl, "hiena_scannerop_parse");
    error = dlerror();
    if (error != NULL) {
	fprintf(stderr, "skeleton-tester: %d\n", error);
	exit(EXIT_FAILURE);
    }

    err = parse(fp, hsp);

    /* test scanner results */
    printf("bytes_read: %d\n", hsp->bytes_read);
    atab_print(a);

    /* cleanup */
    hiena_atab_cleanup(a);
    hiena_scanner_payload_cleanup(hsp);
    dlclose(dl);
    fclose(fp);
    cleanup_hpat(buf);
    exit(EXIT_SUCCESS);
}
