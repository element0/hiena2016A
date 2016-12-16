
struct hiena_accesspath_branch {
    gob general_object_chain_header; /* access by typecasting struct to (gob) */
    Hserialno_t key;
    Hino_t val;
};

void accesspath_branch_cleanup ( struct hiena_accesspath_branch *br ) {
    if( br != NULL )
	free( br );
}

void * accesspath_branches_init ( ) {
    struct hiena_accesspath_branch * br = malloc( sizeof(*br) );
    if( br == NULL ) {
	perror("hiena: accesspath_branches_init: can't allocate accesspath branches.\n");
	return NULL;
    }
    memset(br, 0, sizeof(*br));

    gob_set_cleanup( br, accesspath_branch_cleanup );

    return (void *)br;
}

void accesspath_branches_cleanup ( struct hiena_accesspath_branch * br ) {
    gob_cleanup( br );
}
