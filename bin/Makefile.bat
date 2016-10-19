chcp 437
copy ..\..\bin\Makefile Makefile
mingw32-make
main.exe
mingw32-make clean
del Makefile
rem pause