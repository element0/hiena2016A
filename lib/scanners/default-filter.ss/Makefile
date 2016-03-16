SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .l .y .c .o .so

default-filter.so:	default-filter.l default-filter.y
	bison -d default-filter.y
	flex -o default-filter.yy.c default-filter.l
	gcc -c -fPIC default-filter.tab.c default-filter.yy.c
	gcc -shared -o default-filter.so default-filter.tab.o default-filter.yy.o

default-filter-tester:    default-filter-tester.c
	gcc -rdynamic -o default-filter-tester default-filter-tester.c -ldl

.PHONY : clean install
clean:
	-rm default-filter.yy.[co] default-filter.tab.[cho]

libdir = /usr/local/lib/cosmos/_cosm/types
install: default-filter.so
	mkdir -p $(libdir);
	cp -a ./default-filter.so $(libdir)/.	
