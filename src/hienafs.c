#include "hienafs_baked_config.c"
#include "axpadex.h"

struct hiena_file_system
{
    list_t *serverlib;
    Axpadex *ax;
    Dpakdex *dx;
};


void hienafs_load_domain_servers( struct hiena_file_system *hnfs );

struct hiena_file_system *new_hienafs() {
    struct hiena_file_system *hnfs = malloc(sizeof(*hnfs));
    memset(hnfs, 0, sizeof(*hnfs));

    return hnfs;
}

void hienafs_cleanup( struct hiena_file_system *hnfs ) {
    if( hnfs == NULL ) return;
    if( hnfs->ax != NULL ) axpadex_cleanup(hnfs->ax);
    free( hnfs );
}

int hienafs_init_access_paths( struct hiena_file_system *hnfs ) { /* 1 or 0 */
    if(hnfs == NULL) return 0;
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
    return 0;
}

struct hiena_file_system *init_new_hienafs() {
    struct hiena_file_system *hnfs = malloc(sizeof(*hnfs));
    memset(hnfs, 0, sizeof(*hnfs));

    hienafs_load_domain_servers( hnfs );	// located at /usr/lib/cosmosfs/dpserver
    hienafs_load_rql_module( hnfs );
    hienafs_init_domain_packets( hnfs );
    hienafs_init_lookup_strings( hnfs );
    if(!hienafs_init_access_paths( hnfs )) {
	fprintf(stderr, "init_new_hienafs: hienafs_init_access_paths failed. returning NULL.\n");
	hienafs_cleanup(hnfs);
	return NULL;
    }

    return hnfs;
}

int hienafs_set_dpakroot ( struct hiena_file_system *hnfs, Dpak *dpak ) {
    if(hnfs == NULL || dpak == NULL) return 0;
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
    return 1;
}

int hienafs_parse_cmdline ( int argc, char *argv[] ) { /* == num_source_args */
    if (argc < 2) return 0;

    Dpak *rootdpak = new_dpak(NULL);
    Dpak *newdpak;
    int i = 1;
    int valid_adds = 0;

    if( rootpak == NULL ) {
	perror( "hienafs can't init rootpak, abort.\n" );
	return 0;
    }

    
    while (argv[i][0] != '-' && i != argc) {
	newdpak = new_dpak_with_addr_str( argv[i], strlen(argv[i]), rootdpak );
	if( newdpak != NULL ) {
	    dpak_add_cluster_member( rootdpak, dpak );
	    valid_adds++;
	}
	i++;
    }
    if ( valid_adds > 0 ) {
	hienafs_set_dpakroot( hienafs, rootdpak );
    } else {
	dpak_cleanup( rootdpak );
	return 0;
    }
    return i-1;
}


