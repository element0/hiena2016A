/* HIENA_BOUNDS_T_H */
#ifndef _HIENA_BOUNDS_T_H_
#define _HIENA_BOUNDS_T_H_


/*== OBJECT: bounds_t ==*/

typedef struct ppak_bounds_ob 		  bounds_t;
typedef off_t boundslen_t;

typedef struct ppak_bound_element boundl;
struct ppak_bound_element
{
    int   axis;
    boundslen_t offset;
    boundslen_t len;
};

enum bound_cmp_status
{
    BNDS_SAME, BNDS_INSIDE, BNDS_OUTSIDE, BNDS_INTERSECT, BNDS_DIFF
};

struct ppak_bounds_ob {
    int     c;      /* num array elements */
    boundl *a;      /* array of boundry axi */
    int     pointmatch; /*{ $$ = 0 if no matching performed yet,
			         1 if all offsets have matched so far,
				 2 if one or more offsets have not matched
			}*/
    int     status; /* status from bounds_cmp()
    		          'i' inside
		          'o' outside
		          '=' exact match
		    */
};
/*--------*/

#endif /*!_HIENA_BOUNDS_T_H_*/
