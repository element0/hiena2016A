SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .o .so

filesrvtest: filesrvtest.c filesrv.c
	gcc -Wall filesrvtest.c -o filesrvtest

file.so: filesrv.c
	gcc -I../../../include -Wall -fPIC -shared -o file.so filesrv.c

filesrv: filesrv.c
	gcc -Wall -c filesrv.c -o filesrv

.PHONY : clean install

clean:
	rm filesrv file.so

srvlibdir = $(libdir)/cosmosfs/dserver

install: file.so
	mkdir -p $(srvlibdir);
	cp -a ./file.so $(srvlibdir)/.

