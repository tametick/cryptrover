ifdef DBG
CFLAGS= -std=c99 -O0 -g -Wall -Wextra -pedantic
else
CFLAGS= -std=c99 -O2
endif

ifdef WINDIR
  LDFLAGS= pdc28_ming_w32/pdcurses.a pdc28_ming_w32/panel.a -lm -D__WIN32__
  BIN= $@.exe
  MDPORT_FLAGS=-D__WIN32__
else
  LDFLAGS= -lncurses -lpanel -lm
  BIN= $@
endif

cr: main.c map.o utils.o entities.o items.o io.o mdport.o
	${CC} ${CFLAGS} $? -o $@ ${LDFLAGS}
	strip ${BIN}

mdport.o: mdport.c
	${CC} -c ${MDPORT_FLAGS} -o $@ $? 

astyle:
	astyle --style=java -t -n *.c *.h
ctags:
	ctags -f ~/tags $(shell pwd)/*.c $(shell pwd)/*.h
clean:
	rm -f *.o *~ cr dbg cr.exe
