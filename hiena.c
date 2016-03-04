#define _HIENA_BUILD_
/* turns off testing features of some scannerserver source files */
#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>	/* defines EXIT_FAILURE */
#include <string.h>

#include "src/hiena.h"

/*
   hiena utility
   takes a file and a request string as input
   uses a hard-wired scannerlib path
   -or- an environment variable scannerlib path
   loads the scanner library
   maps the file
   runs the request string
   possibly sucks in standard in
   possibly shoots out standard out
 */

#define SCANLIBPATH "/usr/local/lib/cosmos/_cosm/types"
#define HOSTNAMEMAX 256

struct hiena_util
{
    /* starting environment */
    list_t *serverlib;
    char   *startpwd;
    char   *starthome;
    char   *starthienascanlib;
    char    hostname[HOSTNAMEMAX];
    /* hiena specific */
    int     cmd;
    char   *rootaddr;
    char   *rqstr;
};

static void do_usage() {
    printf("usage: hiena [-ls|-cat] <request string>\n");
}

static int valid_str(char *s) {
    /* TBD would like to make this use pathconf() or sysconf()
       to check that "strlen" is within host system limits */
    if(strlen(s)<1 || s[strlen(s)+1]!='\0') {
	return -1;
    }
    return 1;
}


static dpakserver *determine_server ( addr_t *a, list_t *servers ) { /* NULL | SERVERPTR */
    struct stat sb;

    if(a == NULL || servers == NULL) return NULL;

    /*
       note:  please don't rely solely on 'stat()' to determine type

       */

    if(ppak_is_dud(a->par) && a->bounds == NULL) {
        if(stat( hpat_str_ptr(a->pat), &sb) == -1) {
	    perror("stat");
	    return NULL;
        }
	switch (sb.st_mode & S_IFMT) {
	/* ADDR IS DIR */
	/* addr->par will be a "dud"
	   addr->bounds will be empty
	   and 'stat()' on "addr->pat->buf" will match "S_IFDIR" */
	    case S_IFDIR:
		return list_get_match(servers, server_name_match_op, "localdir");
		break;
	/* ADDR IS FILE */
	/* addr->par will be a "dud"
	   addr->bounds will be empty
	   and 'stat()' on "addr->pat->buf" will match "S_IFREG" */
	    case S_IFREG:
		return list_get_match(servers, server_name_match_op, "localfile");
		break;
	}


	return NULL;
    }

    /* ADDR IS IN-MEMORY DPAK */
    /* addr->frag_t will have something in it
       addr->par    will not be a dud.
       */
    return NULL;
}


static void hiena_util_cleanup(struct hiena_util *hu) {
    if(hu->startpwd != NULL) free(hu->startpwd);
    if(hu->starthome != NULL) free(hu->starthome);
    if(hu->starthienascanlib != NULL) free(hu->starthienascanlib);
    if(hu->rootaddr != NULL) free(hu->rootaddr);
    if(hu->rqstr    != NULL) free(hu->rqstr);
    if(hu->serverlib != NULL) serverlib_cleanup(hu->serverlib);

    if(hu != NULL) free(hu);
}


/***********************************
 *     make new hiena util         *
 ***********************************/
struct hiena_util *new_hiena_util (int argc, char *argv[]) {
    if(argc < 2 || argc > 3 || argv == NULL) {
	do_usage();
	return NULL;
    }

    if(!valid_str(argv[1])) {
	return NULL;
    }
    if(argc == 3 && !valid_str(argv[2])) {
	return NULL;
    }

    /***********************************
     *     malloc hiena_util ob        *
     ***********************************/
    struct hiena_util *hu = malloc(sizeof(*hu));
    if(hu == NULL) {
	//fprintf(stderr, "new_hiena_util: can't malloc, abort.\n");
	fflush(stderr);
	return NULL;
    }
    memset(hu, 0, sizeof(*hu));

    char *fpathrq = NULL;
    
    
    /***********************************
     *     setup server lib            *
     ***********************************/
    hu->serverlib = init_serverlib();
    if(hu->serverlib == NULL) {
	fprintf(stderr, "main: can't init serverlib, abort.\n");
	goto cleanup_and_abort;
    }


    /***********************************
     *          lock envars            *
     ***********************************/
    const char *curpwd          = secure_getenv("PWD");
    const char *curhome         = secure_getenv("HOME");
    const char *curhienascanlib = secure_getenv("HIENA_SCANNERS");

    fflush(stderr);
    if ( curpwd != NULL
       && (hu->startpwd = strndup(curpwd, strlen(curpwd))) == NULL) {
	//fprintf(stderr, "new_hiena_util: can't lock PWD, abort.\n");
	goto cleanup_and_abort;
    }
    if(curhome != NULL &&
	    (hu->starthome = strndup(curhome, strlen(curhome))) == NULL) {
	//fprintf(stderr, "new_hiena_util: can't lock HOME, abort.\n");
	goto cleanup_and_abort;
    }
    if(curhienascanlib != NULL) {
	if((hu->starthienascanlib = strndup(curhienascanlib, strlen(curhienascanlib)))
		== NULL) {
	fprintf(stderr, "new_hiena_util: can't copy curhienascanlib into hu, abort.\n");
	goto cleanup_and_abort;
	}
    }else
    if((hu->starthienascanlib = strndup(SCANLIBPATH, strlen(SCANLIBPATH)))
	    == NULL) {
	fprintf(stderr, "new_hiena_util: can't copy builtin SCANLIBPATH to hu, abort.\n");
	goto cleanup_and_abort;
    }

    if( gethostname( &hu->hostname, HOSTNAMEMAX ) != 0 ) {
	fprintf(stderr, "new_hiena_util: can't get hostname, abort.\n");
	goto cleanup_and_abort;
    }
    hu->hostname[HOSTNAMEMAX-1] = '\0';

    /***********************************
     *          set 'cmd'              *
     ***********************************/

    if(argc == 3) {
	if(strncmp(argv[1], "-ls", 4)) {
	    hu->cmd = 1; /* list */
	}else if(strncmp(argv[1], "-cat", 5)){
	    hu->cmd = 2; /* cat */
	}
	fpathrq = argv[2];
    }else if(argc == 2){
	fpathrq = argv[1];
    }

    /***********************************
     *          set 'rootaddr'         *
     ***********************************/
    if(fpathrq[0] == '/') {
	if((hu->rootaddr = strndup("/", 2)) == NULL) {
	    //fprintf(stderr, "new_hiena_util: can't create '/' addr, abort.\n");
	    goto cleanup_and_abort;
	}
	fpathrq++;
    }else if(fpathrq[0] == '~') {
	if((hu->rootaddr = strndup(hu->starthome, strlen(hu->starthome)))
		== NULL) {
	    //fprintf(stderr, "new_hiena_util: can't strndup $HOME, abort.\n");
	    goto cleanup_and_abort;
	}
	fpathrq++;
    }else{
	if((hu->rootaddr = strndup(hu->startpwd, strlen(hu->startpwd)))
		== NULL) {
	    //fprintf(stderr, "new_hiena_util: can't strndup $PWD, abort.\n");
	    goto cleanup_and_abort;
	}
    }
    /***********************************
     *          set 'rqstr'            *
     ***********************************/
    if((hu->rqstr = strndup(fpathrq, strlen(fpathrq))) == NULL) {
	//fprintf(stderr, "new_hiena_util: can't strndup fpathrq, abort.\n");
	goto cleanup_and_abort;
    }
	
    return hu;
cleanup_and_abort:
    fflush(stderr);
    hiena_util_cleanup(hu);
    return NULL;
}

/***********************************
 *            MAIN                 *
 *                                 *
 ***********************************/
int main (int argc, char *argv[]) {

    /***********************************
     *       init program object       *
     ***********************************/
    int exit_status = EXIT_FAILURE;
    struct hiena_util *hu = new_hiena_util(argc, argv);
    if(hu == NULL) {
	exit(exit_status);
    }
    
    
    /**********************************
     *        load scanlib            *
     **********************************/
    /* TODO: differentiate between scanners and filters */
    /* include 'default-filter' */

    if(hu->starthienascanlib == NULL) {
	printf(stderr, "hiena_util: starthienascanlib unexpectedly not initialized.\n");
    }
    scanlib *slib = load_scanlib_from_path(hu->starthienascanlib);
    if(slib == NULL) {
	fprintf(stderr, "main: can't load scanlib, abort.\n");
	goto cleanup_and_abort_scanlib;
    }
    /* include 'url' 'get' 'ox' 'div' */
    /* include 'cosmosfs/rql/' */
    //scanlib_print(slib);


    
    /***********************************
     *          set root Dpak          *
     ***********************************/


    Dpak *rootpak = new_ppak( NULL );	/* NULL garbage can */
    dpakserver *rootserv = determine_server( hu->rootaddr, hu->serverlib );

    if ( rootpak == NULL || rootserv == NULL ) {
	goto cleanup_and_abort;
    }

    if (  ! ppak_set_addr_str( rootpak, hu->rootaddr )
       || ! ppak_set_server( rootserv )
       || ! ppak_set_filter_name( rootpak, "default-filter", slib )
       || ! ppak_set_hostname( rootpak, &hu->hostname ))
    {
	goto cleanup_and_abort_2;
    }
    
    
    /***********************************
     *      set request pack           *
     ***********************************/
    
    Ppak *request = new_ppak( rootpak );	/* use "rootpak" for garbage collection */
    if ( request == NULL )
	goto cleanup_and_abort_scanlib;


    if (  ! ppak_set_addr_ptr( request, hu->rqstr, strlen(hu->rqstr)+1 )
       || ! ppak_set_server_name( request, "host_mem", hu->serverlib )
       || ! ppak_set_filter_name( request, "default-filter", slib )
       || ! ppak_set_hostname( rootpak, &hu->hostname ))
    {
	goto cleanup_and_abort_request;
    }



    /***********************************
     *       set root access path      *
     ***********************************/

    Axpa *axroot = new_axpa(NULL);
    if ( axroot == NULL )
	goto cleanup_and_abort_request;


    /* for a root 'Axpa', "domain" and "selection" should be the same */

    if (  ! axpa_set_scanners( axroot, slib)
       || ! axpa_set_rql( axroot, request)
       || ! axpa_set_parent_domain( axroot, rootpak)
       || ! axpa_set_domain( axroot, rootpak))
    {
	goto cleanup_and_abort_axpa;
    }
    
    

    
    /**************************************************
     *   set hiena scanner payload to parse request   *
     **************************************************/
    
    hsp_init_globals();

    Hsp *h = hsp_init( request );
    if (  h == NULL )
	goto cleanup_and_abort_axpa;


    if (  ! hsp_set_axpa( h, axroot )
       || ! hsp_set_scanner_name( h, "fudge" ))
        /* must come AFTER hsp_set_axpa() -- b/c it uses Axpa's scannerlib */
    {
	goto cleanup_and_abort_hsp;
    }

    



    /***********************************
     *        perform request          *
     ***********************************/

    Axpa *result = hsp_execute( h );

    if ( result == NULL) {
	fprintf(stderr, "main: hsp_execute() failed, returned NULL,\n");
	goto cleanup_and_abort_hsp;
    }


    /*************************************
     *        perform CLI command        *
     *************************************/

    
    /* "-ls"  */
    if ( hu->cmd == 2 ) {
	printf("LIST:\n");
	axpa_print_dir(result);
	//ppak_list_map(rootpak, 2); /*2==child map*/
	//ppak_list_map(rootpak, 1); /*1== child map */
    }

    /* "-cat" */
    if ( hu->cmd == 1 ) {
	printf("CAT:\n");
	axpa_print_stream(result);
    }


    /*************************************
     *          cleanup                  *
     *************************************/
    //printf("cleaning up...\n");
    exit_status = EXIT_SUCCESS;

cleanup_and_abort_hsp:
    hsp_cleanup(h);
    hsp_globals_cleanup();
cleanup_and_abort_axpa:
    axpa_cleanup(axroot);
cleanup_and_abort_request:
cleanup_and_abort_scanlib:
    scanlib_cleanup(slib);
cleanup_and_abort_2:
    ppak_cleanup_garbage(rootpak);
cleanup_and_abort:
    hiena_util_cleanup(hu);
    //printf("exiting...goodbye.\n");
    exit(exit_status);

}
