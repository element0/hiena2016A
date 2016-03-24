#include <hiena/lookup_module.h>

/* callbacks for flex */

size_t lookup_callbacks_read ( void * buf, size_t count, size_t size, Hframe * h ) {
    return 0;
}

int lookup_callbacks_ferror ( Hframe * h ) {
    return -1;
}

void lookup_callbacks_clearerr ( Hframe * h ) {
}

/* callbacks for bison */

int lookup_callbacks_add_va ( Hframe * h, void * var ) {
    return -1;
}

int lookup_callbacks_sql ( Hframe * h, const char * sqlstr, size_t len ) {
    return -1;
}
