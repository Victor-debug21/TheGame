@echo off


mkdir ..\..\..\buildtest
pushd ..\..\..\buildtest
cl -Zi /W3 /WX /EHsc /nologo j:\TheGame\code\multi-threaded_logic\win32_test.cpp User32.lib gdi32.lib /Fe:test.exe
popd
