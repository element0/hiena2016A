/*_HIENA_AXPA_H_*/
#ifndef _HIENA_AXPA_H_
#define _HIENA_AXPA_H_

#include <gob.h>
#include <progconf/paths.h>

typedef struct hiena_access_path  Axpa;

struct hiena_access_path {
    gob	   general_object_chain_header;		/**< typecast the object as (gob) and then use
					 	     gob functions to access. */
    int    syncflags;		/**< sync flags: 0 == out of sync, 1 == in sync */
    
    struct stat stbuf;		/**< set by 'lookup' module */

    Axpa * parent;


    /** the lookup cache matches request strings which have succeeded
        in previous lookups against the domain.  query the cache with 
	a request stream's GUID and get a access path ino in return.

     */
    void * lookup_cache;  	/* renamed.  was 'branches' */

    char   cascadingconfigdirname[HIENA_CASCADING_CONFIG_DIRNAME_ALLOCATION];
    size_t cascadingconfigdirnamesize;
    void * cascade;		/* stub for cascading configuration */


    void * domain;


    /** scannerlib used to parse the domain's children.
        do not use this to parse domain member of self.
	you must use the parent's access path to parse self's domain.

     */
    void * scannerlib;
};

#endif /*!_HIENA_AXPA_H_*/
