#ifdef RQL_TESTER_C
typedef void Hsp;
#endif

typedef void* yyscan_t;
#define RQL_EXTRA_TYPE Hsp *
int rqllex_init_extra (RQL_EXTRA_TYPE user_defined, yyscan_t* scanner);
int rqllex_destroy (yyscan_t yyscanner );
int rqlparse (yyscan_t *scanner, Hsp *h);

void rqlset_in  (FILE * in_str ,yyscan_t yyscanner );
