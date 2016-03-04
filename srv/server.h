/* SERVER MODULE: generic header
   all server modules include this header.

architecture: linux

   */

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

struct hiena_server_lock {	/* ripped from fcntl.h so we don't import */
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t pid;
};


struct hiena_transaction {
    int   id;        /* temp transaction reference id used by callbacks */
                     /* this is for security.  don't expose process memory to a module. */
    void *own;       /* private state info maintained by server */
    void **ptr;	     /* return buffer malloc'd by server */
    /* callback ops.  security: this should be a clone before passing to here. */
    /* in other words, don't re-use these pointers after passing this struct to a dylib */
    /* only pass it once.  always pass a fresh copy. */
    int (*set_file_type)( mode_t type, int id );
    int (*new_child)( int id );
    int (*child_set_prop)( const char *p_name, void *p_val, int child_id );
    int (*child_set_addr)( void *addr, int child_id );
};


/* standard API for server modules required by hiena-cosmos */

int    server_stat(    void *addr, struct stat **buf, struct hiena_transaction * );
		    /* note **buf is for a stat buffer to be malloc'd by server */
void  *server_open(    void *addr, const char *mode, struct hiena_transaction * );
void   server_lock(    void *object, struct hiena_server_lock, struct hiena_transaction * );
int    server_trylock( void *object, struct hiena_server_lock, struct hiena_transaction * );
size_t server_read(    void **ptr,  size_t size, void *object, struct hiena_transaction * );
size_t server_write(   void  *ptr,  size_t size, void *object, struct hiena_transaction * );
void   server_unlock(  void *object, struct hiena_transaction * );
int    server_close(   void *object, struct hiena_transaction * );





