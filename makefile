# kindly set these env vars for us befure running (DIRSAMPLING prefix optional)
LIBEXECDIR?=$(DIRSAMPLING_LIBEXECDIR)
MAX_INCLUDE?=$(DIRSAMPLING_MAX_INCLUDE)

CC=x86_64-w64-mingw32-gcc
CFLAGS=-std=gnu11 -DWIN_VERSION -DWIN_EXT_VERSION -I"$(MAX_INCLUDE)"
LDFLAGS=-L"$(MAX_INCLUDE)/x64"
LIBS=-lMaxAPI

lib/dirsampling.mxe64 : build/dirsampling.o
	$(CC) -shared -o lib/dirsampling.mxe64 build/dirsampling.o \
		$(LDFLAGS) $(LIBS) 

build/dirsampling.o : src/dirsampling.c
	$(CC) -c $(CFLAGS) -o build/dirsampling.o src/dirsampling.c

.PHONY : install
install :
	cp lib/dirsampling.mxe64 "$(LIBEXECDIR)"

.PHONY : clean
clean :
	rm -rf build/* lib/*

.PHONY : all
all : lib/dirsampling.mxe64 install clean

