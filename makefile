# set as needed
MAX_INCLUDES="/cygdrive/c/Users/mdl1550/Documents/Max 8/Packages/max-sdk-7.3.3/source/c74support/max-includes"
MAX_LIBRARY="/cygdrive/c/Users/mdl1550/Documents/Max 8/Library"

CC=x86_64-w64-mingw32-gcc
CFLAGS=-std=gnu11 -DWIN_VERSION -DWIN_EXT_VERSION -I$(MAX_INCLUDES)
LDFLAGS=-L$(MAX_INCLUDES)/x64
LIBS=-lMaxAPI

lib/dirsampling.mxe64 : build/dirsampling.o
	$(CC) -shared -o lib/dirsampling.mxe64 build/dirsampling.o $(LDFLAGS) $(LIBS)

build/dirsampling.o : src/dirsampling.c
	$(CC) -c $(CFLAGS) -o build/dirsampling.o src/dirsampling.c

.PHONY : install
install :
	cp lib/dirsampling.mxe64 $(MAX_LIBRARY)

.PHONY : clean
clean :
	rm -rf build/* lib/*

.PHONY : all
all : lib/dirsampling.mxe64 install clean

