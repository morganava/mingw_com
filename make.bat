@echo on

echo Clean
del E:\mingw_com\int\* /Q
del E:\mingw_com\out\* /Q

echo Make Client
cl /FoE:\mingw_com\int\ /c /Zi /DUSE_COMPILER_EXCEPTIONS interface_c.c
cl /FoE:\mingw_com\int\ /c /Zi /DUSE_COMPILER_EXCEPTIONS interface_p.c
cl /FoE:\mingw_com\int\ /c /Zi /EHsc client.cpp
link E:\mingw_com\int\interface_c.obj E:\mingw_com\int\interface_p.obj E:\mingw_com\int\client.obj /OUT:E:\mingw_com\out\client.exe /DEBUG:FULL

rem echo Make Server
rem cl /FoE:\mingw_com\int\ /c /Zi /DUSE_COMPILER_EXCEPTIONS interface_s.c
rem cl /FoE:\mingw_com\int\ /c /Zi /EHsc server.cpp
rem link E:\mingw_com\int\interface_s.obj E:\mingw_com\int\server.obj /OUT:E:\mingw_com\out\server.exe /DEBUG:FULL