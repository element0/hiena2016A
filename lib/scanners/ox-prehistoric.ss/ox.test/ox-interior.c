// non-flex c implementation
#include <stdio.h>

#define TABTOSPACE 8

int main (int argc, char *argv[])
{
    char c;
    unsigned int  count, newcount;
    do {
	c = getc(stdin);
	if(c==' ')
	    count++;
	if(c=='\t')
	    count+=TABTOSPACE-(count % TABTOSPACE);
    }while(c==' '||c=='\t');
    newcount = count;


    if(c=='\n') {
	// printf("invalid format: first line was blank.\n");
	return 1;
    }

    do{
	while(c!='\n') {
	    if((c!=' '||c!='\t')&&(newcount<count))
		return 0;
	    fputc(c,stdout);
	    c=getc(stdin);
	}
	fputc(c,stdout);
	fflush(stdout);

	c=getc(stdin);
	newcount=0;

	while((c==' '||c=='\t')&&(newcount<=count)) {
	    if(c==' ')
		newcount++;
	    if(c=='\t')
		newcount+=TABTOSPACE-(count % TABTOSPACE);
	    c = getc(stdin);
	}

    }while(1);

    return 0;
}
