/* HIENA_HIENAFS_C */

#include <sys/stat.h>	/* stat */
#include "../progconf/paths.h"
#include "serverlib.c"
#include "domainstream.c"
#include "accesspathdb.c"

/**
 * The Hiena file system object.
 */
struct hiena_file_system
{
    void * serverlib;	/**< The host resource servers to serve the domain. */
    void * ax;	/**< The access path database. */
    void * dx;	/**< The domain map database. */
    /* TEMPORARY WORKING STRUCTURES */
    struct hiena_domainstream * rootds;
};

struct hiena_file_system *new_hienafs() {
    struct hiena_file_system *hnfs = malloc(sizeof(*hnfs));
    memset(hnfs, 0, sizeof(*hnfs));

    return hnfs;
}

void hienafs_cleanup( struct hiena_file_system *hnfs ) {
    if( hnfs == NULL ) return;
    //if( hnfs->ax != NULL ) axpadex_cleanup(hnfs->ax);
    serverlib_cleanup( hnfs->serverlib );
    domainstream_cleanup( hnfs->rootds );	/**< temporary root ds destructor
						  should be taken care of by
						  cleaning up domainstream database */
    accesspathdb_cleanup( hnfs->ax );
    free( hnfs );
}


void hienafs_config( struct hiena_file_system *hnfs ) {
}

void hienafs_load_domain_servers( struct hiena_file_system *hnfs ) {
    void * sl = serverlib_init( strlen(HIENA_SERVERLIB_PATH), HIENA_SERVERLIB_PATH );
    if( hnfs->serverlib != NULL )
	serverlib_cleanup( hnfs->serverlib );
    hnfs->serverlib = sl;
}

void hienafs_load_rql_module( struct hiena_file_system *hnfs ) {
}

void hienafs_init_lookup_strings( struct hiena_file_system *hnfs ) {
}

int hienafs_init_access_paths( struct hiena_file_system *hnfs ) { /* 1 or 0 */
    if(hnfs == NULL) return 0;
    /*
    if(hnfs->ax != NULL) axpadex_cleanup(hnfs->ax);

    Axpadex *ax  = hnfs->ax = new_axpadex();
    if( ax == NULL) {
	fprintf(stderr, "hienafs_init_access_paths: new_axpadex returned NULL. returning 0.\n");
	return 0;
    }

    Axpa *axroot = ax->axroot = new_axpa( NULL );
    if( axroot == NULL ) {
	fprintf(stderr, "hienafs_init_access_paths: new_axpa returned NULL. returning 0.\n");
	goto abort;
    }

    if(axpadex_add_axpa( ax, axroot ) == 0) {
	fprintf(stderr, "hienafs_init_access_paths: axpadex_add_axpa failed. returning 0.\n");
	goto abort2;
    }

    return 1;

abort2:
    axpa_cleanup(axroot);
abort:
    axpadex_cleanup(ax);
    */
    return 0;
}

void hienafs_init_root_domainstream( struct hiena_file_system * hnfs ) {
    struct hiena_domainstream * ds = domainstream_new();
    if( ds != NULL )
	hnfs->rootds = ds;	/**< temporary place for rootds
				  the appropriate place is inside the 
				  domainstream database */
}

void hienafs_init_domainstream_db ( struct hiena_file_system * hnfs ) {
}

int hienafs_init_accesspath_db ( struct hiena_file_system * hnfs ) {
    if( hnfs == NULL ) return -1;
    Axpadb *ax = accesspathdb_init();
    if( ax == NULL ) return -1;
    hnfs->ax = ax;
    return 1;
}

/**
 * Create and initialize a new file system object.
 * Note: this does not init the dpakroot,
 * you should use 'hienafs_parse_cmdline( argc, argv )' for that.
 */
struct hiena_file_system *hienafs_init () {
    struct hiena_file_system *hnfs = new_hienafs();

    hienafs_config( hnfs ); /* TODO */
    hienafs_load_domain_servers( hnfs );	/* lib paths wired into ../progconf/paths.h */
    hienafs_init_domainstream_db( hnfs );	/**< creates new database
						  and creates root domainstream */
    hienafs_init_accesspath_db( hnfs );		/**< creates new access path database
						  and creates root access path */
    hienafs_load_rql_module( hnfs );
    hienafs_init_lookup_strings( hnfs );
    /*
    if(!hienafs_init_access_paths( hnfs )) {
	fprintf(stderr, "hienafs_init: hienafs_init_access_paths failed. returning NULL.\n");
	hienafs_cleanup(hnfs);
	return NULL;
    }
    */

    return hnfs;
}

int hienafs_set_dpakroot ( struct hiena_file_system * hnfs, void * dpak ) {
    if(hnfs == NULL || dpak == NULL) return 0;
    /*
    if(hnfs->dx != NULL) {
	if( dx->dpakroot != NULL) {
	    dpak->garbage_next = dx->dpakroot;
	    dx->dpakroot = dpak;
	} else {
	    dx->dpakroot = dpak;
	}
    } else {
	return 0;
    }
    */
    return 1;
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
    status = stat(argv[argc-1], &sbuf);

    if( !(S_ISDIR(sbuf.st_mode) || S_ISREG(sbuf.st_mode)) ) {
	printf("hienafs: please use a file or directory as the source.\n");
	hienafs_usage( argv[0] );
	return 0;
    }

    /* SET ADDR OF ROOT DSTREAM */
    /* SET SERVER OF ROOT DSTREAM */

    return 1;
}


