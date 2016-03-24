/* HIENA_LOOKUP_MODULE_H */
#ifndef _HIENA_LOOKUP_MODULE_H_
#define _HIENA_LOOKUP_MODULE_H_

typedef void * yyscan_t;

typedef struct hiena_lookup_frame Hframe;

struct hiena_lookup_ops {
    int (*yyparse)( yyscan_t *, Hframe * );
    int (*yylex_init_extra)( Hframe *, yyscan_t * );
    int (*yylex_destroy)( yyscan_t * );
};

typedef struct hiena_lookup_callbacks {
    /* callbacks for flex */
    size_t (*read)( void * buf, size_t count, size_t size, Hframe * );
    int (*ferror)( Hframe * );
    void (*clearerr)( Hframe * );
    /* callbacks for bison */
    int (*add_va)( Hframe *, void * var );
    int (*sql)( Hframe *, const char * sqlstr, size_t len );
}Hcb;

struct hiena_lookup_frame {
    Hcb * op;
};



#endif /*!_HIENA_LOOKUP_MODULE_H_*/
