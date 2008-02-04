ifdef DBG
CFLAGS= -std=c99 -O0 -g -Wall -Wextra -pedantic -I.
else
CFLAGS= -std=c99 -O2 -I.
endif

ifdef WINDIR
  LDFLAGS= pdc28_ming_w32/pdcurses.a -lm  -D__WIN32__
  BIN= $@.exe
  MDPORT_FLAGS=-D__WIN32__
else
  LDFLAGS= -lncurses -lm
  BIN= $@
endif

cr: cr.c utils.o
	${CC} ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip ${BIN}

mdtest: mdtest.c mdport.o
	${CC} ${CFLAGS} $? -o $@ ${LDFLAGS}

mdport.o: mdport.c
	${CC} -c -Wall -I. ${MDPORT_FLAGS} -o $@ $? 

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o cr dbg cr.exe
