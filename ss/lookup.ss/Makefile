
lookup.so:	lookup.l lookup.y
	bison -d lookup.y
	flex -o lookup.yy.c lookup.l
	gcc -c -fpic lookup.tab.c lookup.yy.c
	gcc -shared -o lookup.so lookup.tab.o lookup.yy.o

lookup-tester:    lookup.l lookup.y
	bison -d lookup.y
	flex -o lookup.yy.c lookup.l
	gcc -o lookup-tester lookup.tab.c lookup.yy.c
