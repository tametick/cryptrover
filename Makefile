ifdef DBG
CFLAGS= -std=c99 -O0 -g -Wall -Wextra -pedantic
else
CFLAGS= -std=c99 -O2
endif

ifdef WINDIR
  LDFLAGS= pdc28_ming_w32/pdcurses.a -lm -D__WIN32__
  BIN= $@.exe
  MDPORT_FLAGS=-D__WIN32__
else
  LDFLAGS= -lncurses -lm
  BIN= $@
endif

cr: cr.c utils.o mdport.o
	${CC} ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip ${BIN}

mdport.o: mdport.c
	${CC} -c ${MDPORT_FLAGS} -o $@ $? 

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
clean:
	rm -f *.o *~ cr dbg cr.exe
