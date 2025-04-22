@echo off


mkdir ..\..\build
pushd ..\..\build
cl -Zi w:\TheGame\code\game.c User32.lib gdi32.lib

popd
