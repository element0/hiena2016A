#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "hpat.h"
//#include "ppak.h"
//#include "accesspath.h"
//#include "sslib.h"
//#include "sqlinterp/rql.tab.h"

#include "../include/hiena/lookup_module.h"
#include <progconf/paths.h>

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_LTDL_H
# include <ltdl.h>
#endif

struct hiena_lookup_module {
    lt_dlhandle handle;
    struct hiena_lookup_ops * ops;
    struct hiena_lookup_callbacks * callbacks;
};


/** MODULE LOADER FUNCS ****************/

#include "lookup_callbacks.c"

static int lookup_module_load ( const char * filename, void * data ) {
    if( filename == NULL || data == NULL ) return -1;

    struct hiena_lookup_module * hlm = data;
    if( hlm->handle != NULL ) return 0;

#if HAVE_LTDL_H

    lt_dlhandle module = lt_dlopenext( filename );
    if( module == NULL ) {
	printf("hiena can't load %s\n", filename );
	return -1;
    }
    printf("lookup_module_load: loaded %s\n", filename);


    hlm->handle = module;

    struct hiena_lookup_ops * op = malloc(sizeof(*op));
    if( op == NULL ) {
	printf("heina can't allocate mem for lookup ops.\n");
	free( hlm );
	lt_dlclose( module );
	return -1;
    }
    memset(op, 0, sizeof(*op));


    op->yyparse = lt_dlsym( module, "yyparse" );
    op->yylex_init_extra = lt_dlsym( module, "yylex_init_extra" );
    op->yylex_destroy = lt_dlsym( module, "yylex_destroy" );

    if( op->yyparse == NULL
     || op->yylex_init_extra == NULL
     || op->yylex_destroy == NULL )
    {
	printf("hienafs: one of the lookup symbols failed to load.\n");
	free( hlm );
	free( op );
	lt_dlclose( module );
	return -1;
    }

    hlm->ops = op;

    struct hiena_lookup_callbacks * callbacks = malloc(sizeof(*callbacks));
    if( callbacks == NULL ) {
	printf("hienafs can't allocate memory for lookup callbacks.\n");
	free( hlm );
	free( op );
	lt_dlclose( module );
	return -1;
    }

    callbacks->read     = lookup_callbacks_read;
    callbacks->ferror   = lookup_callbacks_ferror;
    callbacks->clearerr = lookup_callbacks_clearerr;
    callbacks->add_va   = lookup_callbacks_add_va;
    callbacks->sql      = lookup_callbacks_sql;

    hlm->callbacks = callbacks;

    return 1;
#else
    return -1;
#endif
}

void * lookup_module_init ( const char * searchpath ) {
    if( searchpath == NULL ) {
	printf("lookup_module_init: searchpath is NULL\n");
	return NULL;
    }
    struct hiena_lookup_module * hlm = malloc(sizeof(*hlm));
    if( hlm == NULL ) {
	printf("hiena can't allocate mem for lookup module.\n");
	return NULL;
    }
    memset(hlm, 0, sizeof(*hlm));
    lt_dlforeachfile( searchpath, lookup_module_load, hlm );
    return (void *)hlm;
}

void lookup_module_cleanup ( void * ptr ) {
    struct hiena_lookup_module * hlm = ptr;
    if( hlm == NULL ) return ;
    if( hlm->handle != NULL ) {
	lt_dlclose( hlm->handle );
    }
    if( hlm->ops != NULL ) free( hlm->ops );
    if( hlm->callbacks != NULL ) free( hlm->callbacks );
    free( hlm );
}

struct hiena_lookup_ops * lookup_module_get_ops ( struct hiena_lookup_module * lumod ) {
    if( lumod == NULL ) return NULL;
    return lumod->ops;
}

struct hiena_lookup_callbacks * lookup_module_get_callbacks ( struct hiena_lookup_module * lumod ) {
    if( lumod == NULL ) return NULL;
    return lumod->callbacks;
}
