void *dir_new() {
    return NULL;
}

int dir_addr_set( void *dir, char *addr, char * serversign ) {
    printf("dir_addr_set: dir:%lu, addr:%s server:%s\n", dir, addr, serversign); 
    return 0;
}

int dir_map_add( void *dir, void *map ) {
    printf("dir_map_add: dir:%lu, map:%lu\n", dir, map);
    return 0;
}

int dir_add( void *dst_dir, void *src_dir ) {
    if( dst_dir == NULL || src_dir == NULL )
	return -1;
    printf("dir_add dst_dir:%lu src_dir:%lu\n", dst_dir, src_dir);
    return 0;
}

void *map_new_freeagent( void *buf, size_t len, const char *pname ) {
    char * str = strndup(buf, len);
    printf("map_new_freeagent: %s\n", str);
    free(str);
    return NULL;
}
