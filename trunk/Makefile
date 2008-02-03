CFLAGS= -std=c99 -O2 
CFLAGS_DBG= -std=c99 -O0 -g -Wall -Wextra -pedantic -DDBG
LDFLAGS= -lncurses -lm
LDFLAGS_W32= pdc28_ming_w32/pdcurses.a -lm -I. 

cr: cr.c
	cc ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip $@
cr.exe: cr.c
	cc ${CFLAGS} -mno-cygwin $? -o $@ ${LDFLAGS_W32}
	strip $@
dbg: cr.c
	cc $(CFLAGS_DBG) $? -o $@ $(LDFLAGS)

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o cr dbg
