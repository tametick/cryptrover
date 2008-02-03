CFLAGS= -std=c99 -O2 -I.
ifdef WINDIR
  LDFLAGS= -mno-cygwin pdc28_ming_w32/pdcurses.a -lm  
  BIN= $@.exe
else
  LDFLAGS= -lncurses -lm
  BIN= $@
endif
CFLAGS_DBG= -std=c99 -O0 -g -Wall -Wextra -pedantic -DDBG -I.

cr: cr.c utils.o
	cc ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip ${BIN}

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o cr dbg cr.exe
