# must be set
# MAX_INCLUDES
# MAX_LIBRARY

CC=x86_64-w64-mingw32-gcc
CFLAGS=-std=gnu11 -DWIN_VERSION -DWIN_EXT_VERSION -I"$(MAX_INCLUDES)"
LDFLAGS=-L"$(MAX_INCLUDES)/x64"
LIBS=-lMaxAPI

lib/dirsampling.mxe64 : src/dirsampling.o
	$(CC) -shared -o lib/dirsampling.mxe64 src/dirsampling.o $(LDFLAGS) $(LIBS)

src/dirsampling.o : 

.PHONY : install
install :
	cp lib/dirsampling.mxe64 "$(MAX_LIBRARY)"

.PHONY : clean
clean :
	rm lib/dirsampling.mxe64

.PHONY : all
all : lib/dirsampling.mxe64 install clean

