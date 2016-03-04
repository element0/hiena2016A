SHELL = /bin/bash
.SUFFIXES:
.SUFFIXES: .l .y .c .o .so

hiena: hiena.c libhiena ox fudge
	gcc -ggdb -Wall -o hiena src/libhiena.so hiena.c -ldl

libhiena: rql src/*.[c]
	gcc -ggdb -Wall -fPIC -c src/libhiena.c -o src/libhiena.o 
	gcc -ggdb -Wall -fPIC -shared -Wl,-soname,src/libhiena.so -o src/libhiena.so src/rql-bison/rql.tab.o src/rql-bison/rql.yy.o src/libhiena.o

rql:    src/rql-bison/*.[yl]
	bison -Wnone -d src/rql-bison/rql.y -o src/rql-bison/rql.tab.c
	flex -o src/rql-bison/rql.yy.c src/rql-bison/rql.l
	sed -i 's/YYSTYPE/RQLSTYPE/g' src/rql-bison/rql.yy.c
	gcc -ggdb -fPIC -c src/rql-bison/rql.tab.c -o src/rql-bison/rql.tab.o
	gcc -ggdb -fPIC -c src/rql-bison/rql.yy.c -o src/rql-bison/rql.yy.o

ox:	ss/ox.ss/ox.[yl]
	$(MAKE) -C ss/ox.ss ox.so install

fudge: ss/fudgelang.ss/fudge.[yl]
	$(MAKE) -C ss/fudgelang.ss fudge.so install

.PHONY : clean install



#`pkg-config fuse --cflags --libs`

#test-cosmosfs_t: cosmosfs_t-tester.c src/scanners_system.c src/hpat.c
#	gcc -ggdb -Wall -rdynamic -o cosmosfs_t-tester cosmosfs_t-tester.c -ldl `pkg-config fuse --cflags --libs`
	

#cosmosfs: cosmosfs.c src/scanners_system.c src/hpat.c src/parse_packet.c src/libcosmos.c
#	gcc -ggdb -Wall -rdynamic -o cosmosfs cosmosfs.c -ldl

# depricating
#atab:	src/atab.c
#	gcc -ggdb -Wall -c src/atab.c -o src/atab.o
#html: lib/html.c
#	gcc -fPIC -ggdb -c lib/html.c -o lib/html.o
#	gcc -ggdb -shared -Wl,-soname,lib/html.so -o lib/html.so lib/html.o

