CFLAGS= /MD #/nologo /W4 /EHsc /Za
CC = cl

build: so-cpp.exe

so-cpp.exe: so-cpp.obj hashmap.obj
	$(CC) /Feso-cpp.exe so-cpp.obj hashmap.obj

so-cpp.obj: so-cpp.c
	$(CC) $(CFLAGS) /Foso-cpp.obj /c so-cpp.c

hashmap.obj: hashmap.c hashmap.h
	$(CC) $(CFLAGS) /Fohashmap.obj /c hashmap.c

clean:
	del *.obj *.exe