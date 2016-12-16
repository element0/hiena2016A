
/*== OBJECT: bounds_t ==*/

bounds_t *new_bounds ( int dim ) {
    bounds_t *b = malloc(sizeof(bounds_t));
    if(b == NULL)
	return NULL;
    memset(b,0,sizeof(bounds_t));


    if(dim < 1) dim = 1;

    b->a = malloc(sizeof(boundl)*dim);
    if(b->a == NULL) {
	free(b);
	return NULL;
    }
    memset(b->a,0,sizeof(boundl)*dim);

    return b;
}

void bounds_cleanup ( bounds_t *b ) { /* == TRUE OR FALSE */
    /* MEMORY: 'bounds' allocated by scanner, free'd by Ppak */
    if (b != NULL) {
	free(b->a);
	free(b);
    }
    return 1;
}

boundl *bounds_get_axis_ptr(bounds_t *b, int axis) {
    if(b == NULL || b->a == NULL) return NULL;

    int i;
    for(i = 0; i < b->c; i++) {
	if(b->a[i].axis == axis) {
	    return &b->a[i];
	}
    }
    return NULL;
}

int bounds_set_offset ( bounds_t *b, int axis, off_t offset ) {
    if(b == NULL) return -1;
    boundl *a = bounds_get_axis_ptr(b, axis);
    if(a == NULL) return -1;
    a->offset = offset;
    return 1;
}

int bounds_set_len ( bounds_t *b, int axis, off_t len ) {
    if(b == NULL) return -1;
    boundl *a = bounds_get_axis_ptr(b, axis);
    if(a == NULL) return -1;
    a->len = len;
    return 1;
}

off_t bounds_get_offset(bounds_t *b, int axis) {
    if(b == NULL) return 0;
    boundl *a = bounds_get_axis_ptr(b,axis);
    if(a == NULL) return 0;
    return a->offset;
}

off_t bounds_get_len(bounds_t *b, int axis) {
    if(b == NULL) return 0;
    boundl *a = bounds_get_axis_ptr(b,axis);
    if(a == NULL) return 0;
    return a->len;
}



int bounds_add_axis ( bounds_t *b, int axis, off_t offset, off_t len ) { /* ERROR | SUCCESS */
    if(b == NULL) {
	fprintf(stderr, "bounds_add_axis: bounds_t input NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }
    if(b->a == NULL) {
	/* this shouldn't bet the case if we used new_bounds() to create b */
	/* at this point we only need one array element */
	b->a = malloc(sizeof(boundl));
	if(b->a == NULL) {
	    fprintf(stderr, "bounds_add_axis: can't init b->a, abort.\n");
	    fflush(stderr);
	    return -1;
	}
	b->c = 1;
	b->a[0].axis   = axis;
	b->a[0].offset = offset;
	b->a[0].len    = len;
    }else{

	/* if b->a exists, 
	   we need to find if axis has already been registered */

	boundl *cur_bound = bounds_get_axis_ptr(b, axis);
	if(cur_bound != NULL) {
	    cur_bound->axis   = axis;
	    cur_bound->offset = offset;
	    cur_bound->len    = len;
	    goto bounds_element_is_set;
	}else{

	/* if bounds has not been registered, we need to add an element to the array */

	    /* first we try finding a '0' axis and write to that */

	    cur_bound = bounds_get_axis_ptr(b, 0);
	    if(cur_bound != NULL) {
		cur_bound->axis   = axis;
		cur_bound->offset = offset;
		cur_bound->len    = len;
		goto bounds_element_is_set;
	    }

	}
	
	/* if we didn't find a '0' axis we must extend the array */

	int newbounds_dim = b->c + 1;
	boundl *newbounds = malloc((sizeof *newbounds)*newbounds_dim);
	if(newbounds == NULL) {
	    fprintf(stderr, "ppak_set_bounds: malloc returned NULL, abort.\n");
	    fflush(stderr);
	    return -1;
	}

	boundl *res = memcpy(newbounds, b->a, sizeof(boundl)*b->c);
	if(res != newbounds) {
	    /* an error occured during memcpy */
	    fprintf(stderr, "ppak_set_bounds: memcpy returned bad value, abort.\n");
	    fflush(stderr);
	    return -1;
	}

	/* looks like memcpy worked, swap in new bounds, destroy old bounds */

	boundl *oldbounds = b->a;
	b->a = newbounds;
	free(oldbounds);

	/* set the new element */

	b->a[newbounds_dim-1].axis   = axis;
	b->a[newbounds_dim-1].offset = offset;
	b->a[newbounds_dim-1].len    = len;
	b->c = newbounds_dim;
    }
bounds_element_is_set:
    return 0;
}

/** COMPARING BOUNDARIES **/

/* A note:

   there are two cases when we need to compare bounds:
	1) assigning an object to a map node
	2) comparing objects

   for the assignment case, we are not concerned with 'len'
   we only need to know if 'axis' and 'offset' match
   if they both match we have an offset match on one axis.

   if we have an offset match on all axis, we have a point match between bounds.

   if we have an offset match on all axis, but "len"s differ we have an intersection.

   intersections are provided without a clear purpose,
   but with the thought that they may be useful to someone in the future.

   if we have a offset match on all axis AND a "len" match on all axis,
   we have a true 1:1 bounds match.

 */

int bounds_pointcpy(bounds_t *dest, bounds_t *src) { /* SUCCESS 1 | ERROR -1 */
    if(dest == NULL || src == NULL) {
	fprintf(stderr, "bounds_pointcpy: dest or src can't be NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }
    boundl *old_array = dest->a;

    dest->a = malloc(sizeof(boundl)*src->c);
    if(dest->a == NULL) {
	fprintf(stderr, "bounds_pointcpy: init dest->a == NULL, abort.\n");
	    fflush(stderr);
	return -1;
    }

    if(memcpy(dest->a, src->a, sizeof(boundl)*src->c) == NULL) {
	fprintf(stderr, "bounds_pointcpy: can't memcpy, abort.\n");
	    fflush(stderr);
	return -1;
    }
    dest->c = src->c;
    if(old_array != NULL)
	free(old_array);

    /* erase 'len' component of dest */
    /* (this is a point, not a box) */
    
    int i;
    for(i=0; i < dest->c; i++) {
	dest->a[i].len = 0;
    }

    return 1;
}

bounds_t *bounds_cmp ( bounds_t *subject, bounds_t *control, bounds_t *result ) {

    /* This function compares two bounds_t objects
       and spits out a new bounds_t object
       -- unless "result" not NULL then store result in "result"
       whose internal array elements contain offsets
       from control to subject,
       whose 'pointmatch' is set if subject and control have identical
       'offset' values on all axis,
       whose 'status' flag is set to indicate the type of intersection
       between the full bounds of subject and control.
     */

    if(subject == NULL || control == NULL) {
	fprintf (stderr, "bounds_cmp: subject or control can't be NULL, abort.\n");
	    fflush(stderr);
	return NULL;
    }

    if(result == NULL) result = new_bounds(1);
    if(result == NULL) {
	fprintf (stderr, "bounds_cmp: can't init result object, abort.\n");
	    fflush(stderr);
	return NULL;
    }

    /* step through all axis in subject
       and compare each to control's */

    int i;
    boundl   *ap;	/* axis object pointer */
    boundl   *aps;	/* axis object pointer from subject */
    for(i = 0; i < subject->c; i++) {
	
	/* match axis from subject to control */

	aps = &subject->a[i];

	/* make sure axis is positive */

	if(aps->axis < 1) {
	    fprintf(stderr, "bounds_cmp: subject's axis less than 1, abort.\n");
	    fflush(stderr);
	    goto cleanup_and_abort;
	}
	ap = bounds_get_axis_ptr(control, aps->axis);


	if(ap == NULL) {

	/* control doesn't have same axis */
	/* add result axis with negated value */

	    bounds_add_axis(result, -(aps->axis), 0, 0);

	}else{

	/* control has the same axis */
	/* make offset array element */

	    bounds_add_axis(result,
		    aps->axis,
		    aps->offset - ap->offset,
		    aps->len - ap->len);

	
	    /* using the result from above, calculate pointmatch */
	    /* reminder: this may evaluate during every loop iteration */

	    ap = bounds_get_axis_ptr(result, aps->axis);
	    if(ap->offset == 0) {

		/* offsets match */

		if(result->pointmatch == 0) { 	/* 0 if no matching performed yet */
		    result->pointmatch = 1;	/* 1 b/c all offsets have matched so far */
		}
	    }else{

		/* offsets don't match */

		result->pointmatch = 2;	/* 2 if one or more offsets have not matched */
	    }
	
	    /* calculate boundsmatch */

	    if(ap->len == 0) {
		
		/* exact 'len' match */

		if(result->status == 0)  /* 0 = status hasn't been set yet */
		    result->status = BNDS_SAME;
	    }else{

		/* len's don't match */

		result->status = BNDS_DIFF;
	    }
	}
    }
    return result;
cleanup_and_abort:
    bounds_cleanup(result);
    return NULL;


    /* RESULTS FORMAT */
    /*
       bounds_t
         := count c
	    intersection status
	    array of boundl

       array of boundl
         := ( subject is offset from control on axis A by offset F )+

       subject is offset from control on axis A by offset F
         := axis A, offset F, len 0
	 
       intersection status
         := subject is within control bounds
	 |  subject is not within control bounds
	 |  control is within subject bounds
	 |  subject intersects control bounds

       subject is within control bounds
         := 1

       subject is not within control bounds
         := 2

       control is within subject bounds
         := 3

       subject intersects control bounds
         := 4
     */
}


/*--------*/
