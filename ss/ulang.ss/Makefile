
ulang-tester:    ulang.l ulang.y
	bison -d ulang.y
	flex -o ulang.yy.c ulang.l
	gcc -o ulang-tester ulang.tab.c ulang.yy.c
