
ucmd-tester:    ucmd.l ucmd.y
	bison -d ucmd.y
	flex -o ucmd.yy.c ucmd.l
	gcc -o ucmd-tester ucmd.tab.c ucmd.yy.c
