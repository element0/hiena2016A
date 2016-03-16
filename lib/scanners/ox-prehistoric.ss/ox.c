#include <stdio.h>
#include "hiena.h"

int
placeholder_function(){
    return 100;
}

int
identify(Hnode *n){
    // examines *n
    // and if it can parse n, 
    // it returns an affirmative;
    return 0;
}

int
id_child(char *line){
    if(line[0]!='	'){
	return 1;
    }else{
    	return 0;
    }
}
