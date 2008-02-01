REM cygwin build script. 
REM pdcurses.h should be in your include path (e.g. /usr/include).

cd..
gcc -std=c99 -DCYGWIN -O2 -lm cr.c cygwin/pdcurses.a -o cr
strip cr.exe
