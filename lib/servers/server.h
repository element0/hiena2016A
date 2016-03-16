/* SERVER MODULE: generic header
   all server modules include this header.

architecture: linux

   */

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

struct hiena_server_lock {	/* ripped from fcntl.h so we don't include fcntl.h */
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t pid;
};


struct hiena_server_transaction {
    /* ---- dir ---- */
    void *dir; 
    void *(*dir_new)();
    int (*dir_addr_set)( void *dir, char *addr );
    int (*dir_map_add)( void *dir, void *map );
    int (*dir_add)( void *dst_dir, void *src_dir );
    /* ---- map ---- */
    void *(*map_new_freeagent)( void *buf, size_t len, const char *prop_name );
};


/* standard API for server modules required by hiena-cosmos */

int    server_stat(    void *addr, struct stat *stbuf );
void  *server_open(    void *addr, const char *mode   );
int    server_close(   void *object  );
size_t server_read(   void **ptr, size_t size, void *object, struct hiena_server_transaction * );
size_t server_write(   void *ptr, size_t size, void *object, struct hiena_server_transaction * );
/* TBD */
void   server_lock(    void *object, struct hiena_server_lock *, struct hiena_server_transaction * );
int    server_trylock( void *object, struct hiena_server_lock *, struct hiena_server_transaction * );
void   server_unlock(  void *object, struct hiena_server_transaction * );




