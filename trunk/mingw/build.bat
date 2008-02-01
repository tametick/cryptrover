REM mingw build script. 
REM pdcurses.h should be in your include path (e.g. /usr/include).

cd..
gcc -std=c99 -DWIN32 -mno-cygwin -O2 -lm cr.c mingw/pdcurses.a -o cr
strip cr.exe
