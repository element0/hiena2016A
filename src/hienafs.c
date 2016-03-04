#include "hienafs_baked_config.c"

struct hiena_file_system
{
    list_t *serverlib;
    Axpa *rootax;
};


void hienafs_load_domain_servers( struct hiena_file_system *hnfs );

struct hiena_file_system *new_hienafs() {
    struct hiena_file_system *hnfs = malloc(sizeof(*hnfs));
    memset(hnfs, 0, sizeof(*hnfs));

    return hnfs;
}

struct hiena_file_system *init_new_hienafs() {
    struct hiena_file_system *hnfs = malloc(sizeof(*hnfs));
    memset(hnfs, 0, sizeof(*hnfs));

    hienafs_load_domain_servers( hnfs );	// located at /usr/lib/cosmosfs/dpserver
    hienafs_load_rql_module( hnfs );
    hienafs_init_domain_packets( hnfs );
    hienafs_init_lookup_strings( hnfs );
    hienafs_init_access_paths( hnfs );

    return hnfs;
}

void cleanup_hienafs( struct hiena_file_system *hnfs ) {
    if( hnfs != NULL )
	free( hnfs );
}

int hienafs_parse_cmdline( int argc, char *argv[] ) { /* == num_source_args */
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
	hienafs_set_root_dpak( hienafs, rootdpak );
    } else {
	dpak_cleanup( rootdpak );
	return 0;
    }
    return i-1;
}


