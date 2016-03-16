dir: dir.y
	bison -d dir.y
	flex -o dir.yy.c dir.l
	gcc -o dir dir.tab.c dir.yy.c
