/* HIENA_DOMAIN_HANDLE_H */
#ifndef _HIENA_DOMAIN_HANDLE_H_
#define _HIENA_DOMAIN_HANDLE_H_


/* FRAGBUF */

void * fragbuf_init ( void * ptr, size_t len );
void * fragbuf_cleanup ();
void * fragbuf_read ();

/* MAP */

void * map_init ();
void * map_cleanup ();
void * map_new_object ();
void * map_seek_object ();

/* OBJECT INDEX */

void * objectindex_init ();
void * objectindex_cleanup ();
void * objectindex_new_object ( void * ob_index, void * ptr, size_t len );
void * objectindex_add_object ();
void * objectindex_get_object ();	/* by grammar rulename */

/* NAMED INDEX */

void * namedindex_init ();
void * namedindex_cleanup ();
void * namedindex_name_object ();
void * namedindex_unset_name ();
void * namedindex_get_object ();

/* CHILD TABLE */

void * childtab_init ();
void * childtab_cleanup ();
void * childtab_add_child ();
void * childtab_get_child ();
void * childtab_firstchild ();
void * childtab_nextchild ();
void * childtab_prevchild ();
void * childtab_lastchild ();
void * childtab_seek_child ();

#endif /*!_HIENA_DOMAIN_HANDLE_H_*/
