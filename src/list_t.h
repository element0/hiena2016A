/* HIENA_LIST_T_H */
#ifndef _HIENA_LIST_T_H_
#define _HIENA_LIST_T_H_

typedef struct ppak_list_element list_t;
struct ppak_list_element
{
    list_t *prev;
    list_t *next;

    void *ob;
};

list_t *new_list      ( list_t * );
list_t *new_list_t    ( void * );
int     list_t_cleanup( list_t * );
int	list_verify   ( list_t * );
list_t *list_get_next ( list_t * );
void    list_set_ob   ( list_t *, void *ob );
void   *list_get_ob   ( list_t * );
int     list_t_add_ob ( list_t *, void *ob );
void   *list_get_match( list_t *, int(*)(void *, void *), void * );
void    list_cleanup_all( list_t *, void(*)(void *) );
int     list_t_cleanup_garbage( list_t * );

#endif /*!_HIENA_LIST_T_H_*/
