.DEFAULT_GOAL := all
MIDL = /media/ssd/Code/Tor/wine/tools/widl/widl
MIDL_FLAGS = --win32 -m32 -I ~/Code/Tor/bin/mingw-w64/i686-w64-mingw32/include -Oicf
GENERATED_SRC = dlldata.c foo.h foo_p.c foo_i.c foo_c.c foo_r.rgs foo.tlb fooproxy.res
CXX = i686-w64-mingw32-g++
CC = i686-w64-mingw32-gcc
WINDRES = i686-w64-mingw32-windres
DLLTOOL = i686-w64-mingw32-dlltool

all: idl proxy server

idl: foo.idl
	$(MIDL) $(MIDL_FLAGS) foo.idl

proxy: idl fooproxy.rc fooproxy.def
	$(WINDRES) fooproxy.rc -O coff -o fooproxy.res
	$(DLLTOOL) -d fooproxy.def -e fooproxy_def.o
	$(CC) -c -DREGISTER_PROXY_DLL dlldata.c
	$(CC) -c -DREGISTER_PROXY_DLL foo_p.c
	$(CC) -c -DREGISTER_PROXY_DLL foo_i.c
	$(CC) -Wl,--enable-stdcall-fixup -shared fooproxy.res fooproxy_def.o dlldata.o foo_p.o foo_i.o -static-libgcc -lrpcrt4 -o fooproxy.dll


server: idl server.cpp
	$(CXX) -c --std=c++17 server.cpp
	$(CC) -c -DREGISTER_PROXY_DLL foo_i.c
	$(CXX) foo_i.o server.o -static-libgcc -static-libstdc++ -luuid -lole32 -loleaut32 -o server.exe

clean:
	rm -f $(GENERATED_SRC) *.o *.dll *.exe
