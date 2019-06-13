@echo on

rem this needs to be run as admin
rem you may run into issues if hosted on network share

rem add CLSID info for local server exe to regsiter
regedit.exe /S foo.reg

rem register the proxy dll
regsvr32 fooproxy.dll
