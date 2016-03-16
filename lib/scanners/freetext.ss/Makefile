
freetext-tester:    freetext.l freetext.y
	bison -d freetext.y
	flex -o freetext.yy.c freetext.l
	gcc -o freetext-tester freetext.tab.c freetext.yy.c
