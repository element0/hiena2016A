
json:		json.l json.y
	bison -d json.y
	flex -o json.yy.c json.l
	gcc -o json json.tab.c json.yy.c

json.so:	json.l json.y
	bison -d json.y
	flex -o json.yy.c json.l
	gcc -c -fPIC json.tab.c json.yy.c
	gcc -shared -o json.so json.tab.o json.yy.o

json-tester:    json-tester.c
	gcc -rdynamic -o json-tester json-tester.c -ldl
