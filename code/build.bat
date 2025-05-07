@echo off


mkdir ..\..\build
pushd ..\..\build
cl -Zi /W4 /WX /EHsc /nologo j:\TheGame\code\game.c User32.lib gdi32.lib /Fe:game.exe


popd
