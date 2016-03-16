
lookup-tester:    lookup.l lookup.y
	bison -d lookup.y
	flex -o lookup.yy.c lookup.l
	gcc -o lookup-tester lookup.tab.c lookup.yy.c
