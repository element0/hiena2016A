rql-tester: *.[ylcho]
	bison -Wnone -d rql.y
	flex -o rql.yy.c rql.l
	sed -i 's/YYSTYPE/RQLSTYPE/g' rql.yy.c
	gcc -c rql.tab.c rql.yy.c
	gcc -o rql-tester rql.tab.o rql.yy.o rql-tester.c

clean:
	rm rql.yy.* rql.tab*
