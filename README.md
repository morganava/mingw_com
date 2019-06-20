# mingw COM server/client

This project provides a barebones out-of-proc COM server and client built with mingw. You may find it useful as a start to a windows COM project or for debugging widl output.

# Building

Just do **make** in the root project directory. Depends mingw the toolchain to run properly and an up-to-date widl compiler from wine. You'll need a version of wine newer than at least 4.10 (at time of this writing, the patches required for this project to work properly have not been merged, see this wine branch with the required widl fixes: https://github.com/pospeselr/wine/tree/winedev

# Usage

Register the server executable and the proxy dll with the provided registerFoo.bat batch file, then run client.exe. If properly registered, server.exe will be spawned by svchost and your client calls will be remoted to it.