CFLAGS= -std=c99 -O2 
CFLAGS_DBG= -std=c99 -O0 -g -Wall -Wextra -pedantic -DDBG
LDFLAGS= -lncurses -lm

cr: cr.c
	cc ${CFLAGS} $? -o $@ ${LDFLAGS}
dbg: cr.c
	cc $(CFLAGS_DBG) $? -o $@ $(LDFLAGS)
cr.exe: cr.c
	cc ${CFLAGS} -mno-cygwin $? -o $@ -lm mingw/pdcurses.a

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o cr dbg
