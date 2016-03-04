default:
#does nothing

test-stringcache: test-stringcache.c ../src/stringcache.c ../src/stringcache.h
	gcc -Wall -I/usr/include -o test-stringcache test-stringcache.c \
	    -L/usr/lib/x86_64-linux-gnu -lkyotocabinet -llzma -llzo2 -lz -lrt -lpthread -lm -lc
