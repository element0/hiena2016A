/* HIENA_SERVER_MODULE_H */
#ifndef _HIENA_SERVER_MODULE_H_
#define _HIENA_SERVER_MODULE_H_

/** @file server_module.h
 *
 * all server modules include this header.
 * hiena includes this header.
 *
 */

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define HIENA_FILEMODE_CHAR_MAX 50

/** Generic address structure for hiena domain objects
 */
typedef struct hiena_addr {
    void * serverlib;
    void * serversign;
    size_t serversign_len;
    void * domain_ptr;
    void * protocol_addr;
    size_t protocol_addr_len;
}addr_t;

/** File Handle Envelope to standardize all server modules from the Hiena's POV.
 */
struct hiena_source_stream_handle {
    void * op;	/* hiena_sourceops */
    void * fh;  /* internal_file_handle native to server */
};

/** Sample locking mechanism ripped from fcntl.h in case we need to implement
  something like this -- or exactly this -- in the future.
 */
struct hiena_source_lock {	/* ripped from fcntl.h so we don't include fcntl.h */
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t pid;
};

/** Callbacks provided by hiena to enable the server to add objects to the map
  or directory structures of the current domain cell.
 */
struct hiena_source_callbacks {
    void * dir; 
    /* ---- NEW! ---- */
    /* ---- 2016/04/11:  cross-server module operations ---- */
    int    (*hiena_stat) (void *, struct stat *);
    struct hiena_source_stream_handle *
           (*hiena_open) (addr_t *, size_t, const char *, struct hiena_source_callbacks *);
    int    (*hiena_close)(void *, struct hiena_source_callbacks *);
    size_t (*hiena_read) (void **, size_t, void *, struct hiena_source_callbacks *);
    size_t (*hiena_write)(void *,  size_t, void *, struct hiena_source_callbacks *);
    int    (*hiena_seek) (void *, off_t offset, int whence);
    /* ---- pre April 2016 ---- */
    void * (*dir_new_child)( void * d_name, size_t d_namelen, void * addr, size_t addrlen, char * serversign );
    /* ---- dir OLD! ---- */
    void *(*dir_new)();
    int (*dir_addr_set)( void * dir, char * addr, char * serversign );
    int (*dir_map_add)( void *dir, void *map );
    int (*dir_add)( void *dst_dir, void *src_dir );
    /* ---- map OLD! ---- */
    void *(*map_new_freeagent)( void *buf, size_t len, const char *prop_name );
};

/** Standard API for server modules required by hiena-cosmos */
struct hiena_sourceops {
    int	   (*stat)   (void *addr, struct stat *stbuf);
    void  *(*open)   (void *addr, size_t size, const char *mode, struct hiena_source_callbacks *);
    int    (*close)  (void *object, struct hiena_source_callbacks *);
    size_t (*read)   (void **ptr, size_t size, void *object, struct hiena_source_callbacks *);
    size_t (*write)  (void *ptr,  size_t size, void *object, struct hiena_source_callbacks *);
    int    (*seek)   (void *fh, off_t offset, int whence);
    /* Suggested by Flex generated scanners in YY_INPUT -- from flex's yy.c skeleton */
    int    (*ferror) (void *yyextra);
    void   (*clearerr)(void *yyextra);
    /* TBD */
    void   (*lock)   (void *object, struct hiena_source_lock *, struct hiena_source_callbacks * );
    int    (*trylock)(void *object, struct hiena_source_lock *, struct hiena_source_callbacks * );
    void   (*unlock) (void *object, struct hiena_source_callbacks * );
};


#endif /*!_HIENA_SERVER_MODULE_H_*/
