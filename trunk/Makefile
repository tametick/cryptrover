ifdef WINDIR
  CFLAGS= -std=c99 -O2 -mno-cygwin
  LDFLAGS= pdc28_ming_w32/pdcurses.a -lm -I. 
  BIN= $@.exe
else
  CFLAGS= -std=c99 -O2 
  LDFLAGS= -lncurses -lm
  BIN= $@
endif
CFLAGS_DBG= -std=c99 -O0 -g -Wall -Wextra -pedantic -DDBG

cr: cr.c
	cc ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip ${BIN}
dbg: cr.c
	cc $(CFLAGS_DBG) $? -o $@ $(LDFLAGS)

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o cr dbg cr.exe
