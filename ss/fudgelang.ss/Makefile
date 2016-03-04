SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .l .y .c .o .so

fudge.so:	fudge.l fudge.y ../../src/libhiena.o
	bison -Wnone -d fudge.y
	flex -o fudge.yy.c fudge.l
	gcc -ggdb -c -fPIC fudge.tab.c fudge.yy.c
	gcc -ggdb -shared -o fudge.so fudge.tab.o fudge.yy.o ../../src/libhiena.o

fudge:		fudge.l fudge.y
	bison -d fudge.y
	flex -o fudge.yy.c fudge.l
	gcc -ggdb -o fudge fudge.tab.c fudge.yy.c

fudge-auto:	fudge.y fudge.l

fudge-tester:    fudge-tester.c fudge.so ../../src/libhiena.c
	gcc -ggdb -o fudge-tester fudge-tester.c -ldl

.PHONY : clean install
clean:
	-rm fudge.yy.[co] fudge.tab.[cho]

libdir = /usr/local/lib/cosmos/_cosm/types
install: fudge.so
	mkdir -p $(libdir);
	cp -a ./fudge.so $(libdir)/.	
