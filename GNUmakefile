CFLAGS= -std=c89 -Wall -g -ansi -pedantic

build: so-cpp

so-cpp: so-cpp.o hashmap.o
	$(CC) $(CFLAGS) so-cpp.o hashmap.o -o $@

so-cpp.o: so-cpp.c
	$(CC) $(CFLAGS) -c so-cpp.c -o $@

hashmap.o: hashmap.c
	$(CC) $(CFLAGS) -c hashmap.c -o $@

clean:
	rm -f *.o so-cpp

.PHONY: clean