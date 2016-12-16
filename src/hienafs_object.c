/* HIENA_HIENAFS_C */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../progconf/paths.h"		/**< compiled program configuration:  install paths */

#include "mapping.c"
#include "domaincelldb.c"
#include "sourcelib.c"
#include "accesspathdb.c"
#include "lookup_module.c"
#include "lookup_stringsdb.c"
#include "lookup_frame.c"
#include "scanlib.c"


/**
 * The Hiena file system object.
 */
struct hiena_file_system
{
    void * sourcelib;	/**< The host resource sources to serve the domain. */
    void * ax;	/**< The access path database. */
    void * dx;	/**< The domain stream database. */
    void * lookup; /**< The lookup module. */
    void * lookupstrings;  /**< The lookup strings cache. */
    /* TEMPORARY WORKING STRUCTURES */
    struct hiena_domaincell * rootds;
};

struct hiena_file_system *new_hienafs() {
    struct hiena_file_system *hnfs = malloc(sizeof(*hnfs));
    memset(hnfs, 0, sizeof(*hnfs));

    return hnfs;
}

void hienafs_cleanup( struct hiena_file_system *hnfs ) {
    if( hnfs == NULL ) return;
    //if( hnfs->ax != NULL ) axpadex_cleanup(hnfs->ax);
    sourcelib_cleanup( hnfs->sourcelib );
    accesspathdb_cleanup( hnfs->ax );
    domaincelldb_cleanup( hnfs->dx );
    lookup_module_cleanup( hnfs->lookup );
    lookupstringsdb_cleanup( hnfs->lookupstrings );
#if HAVE_LTDL_H
    lt_dlexit();
#endif
    free( hnfs );
}


void hienafs_config( struct hiena_file_system *hnfs ) {
}


void hienafs_load_domain_sources( struct hiena_file_system *hnfs ) {
    void * sl = sourcelib_init( strlen(HIENA_SERVERLIB_PATH), HIENA_SERVERLIB_PATH );
    if( hnfs->sourcelib != NULL )
	sourcelib_cleanup( hnfs->sourcelib );
    hnfs->sourcelib = sl;
}


void hienafs_load_lookup_module( struct hiena_file_system *hnfs ) {
    if( hnfs == NULL ) return;
    if( hnfs->lookup != NULL ) {
	lookup_module_cleanup( hnfs->lookup );
    }
    hnfs->lookup = lookup_module_init( HIENA_LOOKUP_MODULE_PATH );
}


void hienafs_init_lookup_strings( struct hiena_file_system *hnfs ) {
    if( hnfs == NULL ) return;
    if( hnfs->lookupstrings != NULL ) {
	lookupstringsdb_cleanup( hnfs->lookupstrings );
    }
    hnfs->lookupstrings = lookupstringsdb_init();
}


int hienafs_init_domaincell_db ( struct hiena_file_system * hnfs ) {
    if( hnfs == NULL ) return 0;

    hnfs->dx = domaincelldb_init();

    if( hnfs->dx == NULL ) return 0;
    return 1;
}


int hienafs_init_accesspathdb ( struct hiena_file_system * hnfs ) {
    if( hnfs == NULL ) return 0;

    hnfs->ax = accesspathdb_init();

    if( hnfs->ax == NULL ) return 0;
    return 1;
}


/**
 * Create and initialize a new file system object.
 * Note: this does not init the dpakroot,
 * you should use 'hienafs_parse_cmdline( argc, argv )' for that.
 *
 */

struct hiena_file_system * hienafs_init ( ) {
    int err = 0;
    struct hiena_file_system *hnfs = new_hienafs();

    hienafs_config( hnfs ); 			/* TODO */
    hienafs_load_domain_sources( hnfs );	/**< lib paths wired into ../progconf/paths.h */
    hienafs_init_domaincell_db( hnfs );	/**< creates new database and root domaincell */

    /** note: root domain's address will be set by hienafs_parse_cmdline */


    /** init accesspath database.
        creates root and proto accesspath nodes.

     */
    if( hienafs_init_accesspathdb( hnfs ) == 0 )/**< new access path database and root access path */
    { 
	perror("hiena:hienafs_init: can't init accesspathdb.\n");
	goto abort;
    }


    /** init proto access path
        this is required for scannerlib cascading.  will be inherited by access path root.

     */
    Axpa * axproto = accesspathdb_get_axproto( hnfs->ax );
    scanlib * scanlib = scanlib_load_from_path( HIENA_PROTO_SCANLIB_PATH );	/* TEMP TODO */
    if( scanlib == NULL ) {
	perror("hienafs_init: can't load proto scanner library.\n");
	goto abort;
    }
    printf("hienafs_init: loaded proto scanlib OK.\n");
    scanlib_print( scanlib );

    if( accesspath_set_scannerlib( axproto, scanlib ) == 0 ) {
	perror("hienafs_init: can't set accesspath's scanner library.\n");
	goto abort;
    }
    printf("hienafs_init: added proto scanlib to proto access path.\n");
    if( accesspath_set_syncflags( axproto, 1 ) == -1 ) {
	perror("hienafs_init: can't set syncronization flags on proto access path.\n");
	goto abort;
    }





    /** init root access path
        - add root domain set to root accesspath
	- wait to add scanlib, it will be loaded on accesspath refresh

     */
    Axpa * axroot = accesspathdb_get_accesspath( hnfs->ax, 1 );
    Hds  * dsroot = domaincelldb_get_domaincell( hnfs->dx, 1 );
    if( accesspath_set_domain( axroot, dsroot ) == 0 ) {
	perror("hiena: hienafs_init: can't add dsroot to axroot.\n");
	goto abort;
    }
    printf("hienafs_init: added root domain to root access path.\n");


    /** init lookup language subsystem

     */
    hienafs_load_lookup_module( hnfs );
    hienafs_init_lookup_strings( hnfs );

    return hnfs;
abort:
    hienafs_cleanup( hnfs );
    return NULL;
}


void hienafs_usage( char * execname ) {
    printf("usage: %s [fuseargs] mountpoint sourceurl\n", execname );
}


int hienafs_parse_cmdline ( struct hiena_file_system * hnfs, int argc, char *argv[] ) { /* == num_source_args */
    if (argc < 2) {
	hienafs_usage( argv[0] );
	return 0;
    }

    struct stat sbuf;
    int status;

    /* GET BIND SOURCES */
    /* TODO: when we offer multiple bind-mounts, they will be set here */
    char * sourcepath = argv[argc-1];

    status = stat( sourcepath, &sbuf );

    if( !(S_ISDIR(sbuf.st_mode) || S_ISREG(sbuf.st_mode)) ) {
	printf("hienafs: please use a file or directory as the source.\n");
	hienafs_usage( argv[0] );
	return 0;
    }

    /* BIND */
    /* SET ADDR OF ROOT DSTREAM */

    Hds * dsroot = domaincelldb_get_domaincell( hnfs->dx, 1 );
    if( dsroot == NULL ) {
	perror("heinafs_parse_cmdline: can't get root domaincell.\n");
	return 0;
    }

    char * copypath = strndup( sourcepath, strlen( sourcepath ) );
    if( copypath == NULL ) {
	perror("hienafs_parse_cmdline: can't copy sourcepath.\n");
	return 0;
    }
    if( domaincell_set_address( dsroot, copypath, free ) == -1 ) {
	perror("heinafs_parse_cmdline: can't set address.\n");
	free( copypath );
	return 0;
    }
    /* SET SERVER OF ROOT DSTREAM */
    if( domaincell_set_source( dsroot, "file", NULL ) == -1 ) {
	perror("hienafs_parse_cmdline: can't set domain source.\n");
	free( copypath );
	return 0;
    }


    return 1;
}

struct hiena_lookup_ops * hienafs_get_lookup_ops ( struct hiena_file_system * hnfs ) {
    if( hnfs == NULL ) return NULL;
    if( hnfs->lookup == NULL ) return NULL;

    return lookup_module_get_ops( hnfs->lookup );
}
