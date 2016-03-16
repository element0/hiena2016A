config:	config.l
	bison -d config.y
	flex -o config.yy.c config.l
	gcc -o config-scanner config.tab.c config.yy.c -lfl
