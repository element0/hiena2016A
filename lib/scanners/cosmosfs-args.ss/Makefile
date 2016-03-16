
cosmosfs-args.so:   cosmosfs-args.y cosmosfs-args.l
	bison -d cosmosfs-args.y
	flex -o cosmosfs-args.yy.c cosmosfs-args.l
	gcc -c -fpic cosmosfs-args.tab.c cosmosfs-args.yy.c
	gcc -shared -o cosmosfs-args.so cosmosfs-args.tab.o cosmosfs-args.yy.o

cosmosfs-args_v1.so: cosmosfs-args.hiena.c
	gcc -c -FPIC cosmosfs-args.hiena.c
	gcc -shared -o cosmosfs-args_v1.so cosmosfs-args.hiena.o

cosmosfs-args-tester:	cosmosfs-args-tester.c
	gcc -rdynamic -o cosmosfs-args-tester cosmosfs-args-tester.c -ldl

