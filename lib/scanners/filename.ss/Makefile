
filename-tester:    filename.l filename.y
	bison -d filename.y
	flex -o filename.yy.c filename.l
	gcc -o filename-tester filename.tab.c filename.yy.c
