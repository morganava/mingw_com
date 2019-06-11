.DEFAULT_GOAL := all
MIDL = /media/ssd/Code/Tor/wine/tools/widl/widl
MIDL_FLAGS = --win32 -m32 -I ~/Code/Tor/bin/mingw-w64/i686-w64-mingw32/include -Oicf
GENERATED_SRC = dlldata.c foo.h foo_p.c foo_i.c foo_c.c foo_r.rgs foo.tlb fooproxy.res
CXX = i686-w64-mingw32-g++
CC = i686-w64-mingw32-gcc
WINDRES = i686-w64-mingw32-windres

all: clean idl proxy

idl: foo.idl
	$(MIDL) $(MIDL_FLAGS) foo.idl

proxy: idl fooproxy.rc
	$(WINDRES) fooproxy.rc -O coff -o fooproxy.res
	$(CC) -c dlldata.c
	$(CC) -c foo_p.c
	$(CC) -c foo_i.c
	$(CC) -shared dlldata.o foo_p.o foo_i.o fooproxy.res  -lrpcrt4 -static-libgcc -o fooproxy.dll

clean:
	rm -f $(GENERATED_SRC) *.o *.dll *.exe
