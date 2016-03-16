
number-tester:    number.l number.y
	bison -d number.y
	flex -o number.yy.c number.l
	gcc -o number-tester number.tab.c number.yy.c
