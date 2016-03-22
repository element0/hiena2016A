/* HIENA_SERVER_MODULE_H */
#ifndef _HIENA_SERVER_MODULE_H_
#define _HIENA_SERVER_MODULE_H_

/** \file server_module.h
 *
 * all server modules include this header.
 *
 */

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/** Sample locking mechanism ripped from fcntl.h in case we need to implement
  something like this -- or exactly this -- in the future.
 */
struct hiena_server_lock {	/* ripped from fcntl.h so we don't include fcntl.h */
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t pid;
};
/** Callbacks provided by hiena to enable the server to add objects to the map
  or directory structures of the current domain parcel.
 */
struct hiena_server_callbacks {	/* prev labeled hiena_server_callbacks */
    /* ---- dir ---- */
    void *dir; 
    void *(*dir_new)();
    int (*dir_addr_set)( void * dir, char * addr, char * serversign );
    int (*dir_map_add)( void *dir, void *map );
    int (*dir_add)( void *dst_dir, void *src_dir );
    /* ---- map ---- */
    void *(*map_new_freeagent)( void *buf, size_t len, const char *prop_name );
};

/** Standard API for server modules required by hiena-cosmos */
struct hiena_serverops {
    int	   (*stat)   (void *addr, struct stat *stbuf);
    void  *(*open)   (void *addr, const char *mode);
    int    (*close)  (void *object );
    size_t (*read)   (void **ptr, size_t size, void *object, struct hiena_server_callbacks *);
    size_t (*write)  (void *ptr,  size_t size, void *object, struct hiena_server_callbacks *);
    /* TBD */
    void   (*lock)   (void *object, struct hiena_server_lock *, struct hiena_server_callbacks * );
    int    (*trylock)(void *object, struct hiena_server_lock *, struct hiena_server_callbacks * );
    void   (*unlock) (void *object, struct hiena_server_callbacks * );
};


#endif /*!_HIENA_SERVER_MODULE_H_*/
