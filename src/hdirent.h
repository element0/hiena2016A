/* HIENA_DIRECTORY_ENTRY_H */

#ifndef _HIENA_DIRECTORY_ENTRY_H_
#define _HIENA_DIRECTORY_ENTRY_H_

typedef struct hiena_directory_entry {
    char * name_str;
    size_t name_len;
    void * addr_buf;
    size_t addr_len;
}hdirent_t;

#endif /*!_HIENA_DIRECTORY_ENTRY_H_*/
