ifdef DBG
CFLAGS= -std=c99 -O0 -g -Wall -Wextra -pedantic -I.
else
CFLAGS= -std=c99 -O2 -I.
endif

ifdef WINDIR
  LDFLAGS= -mno-cygwin pdc28_ming_w32/pdcurses.a -lm  
  BIN= $@.exe
else
  LDFLAGS= -lncurses -lm
  BIN= $@
endif

cr: cr.c utils.o
	cc ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip ${BIN}

mdtest: mdtest.c mdport.o

mdport.o: mdport.c
	cc -g -c -o $@ $?

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o cr dbg cr.exe
