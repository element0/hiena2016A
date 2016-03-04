SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .l .y .c .o .so

skeleton.so:	skeleton.l skeleton.y
	bison -d skeleton.y
	flex -o skeleton.yy.c skeleton.l
	gcc -c -fPIC skeleton.tab.c skeleton.yy.c
	gcc -shared -o skeleton.so skeleton.tab.o skeleton.yy.o

skeleton-tester:    skeleton-tester.c
	gcc -rdynamic -o skeleton-tester skeleton-tester.c -ldl

.PHONY : clean install
clean:
	-rm skeleton.yy.[co] skeleton.tab.[cho]

libdir = /usr/local/lib/cosmos/_cosm/types
install: skeleton.so
	mkdir -p $(libdir);
	cp -a ./skeleton.so $(libdir)/.	
