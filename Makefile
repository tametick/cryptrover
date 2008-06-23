ifdef DBG
CFLAGS+= -std=c99 -O0 -g -Wall -Wextra -pedantic
else
CFLAGS+= -std=c99 -Os
endif

ifdef CROSS
  WINDIR=1
  CC= i586-mingw32msvc-gcc
endif

ifdef WINDIR
  LDFLAGS+= pdc33_ming_w32/pdcurses.a -lm 
  CFLAGS+= -D__PDC__ -D__WIN32__
  BIN= $@.exe
  MDPORT_FLAGS=-D__PDC__ -D__WIN32__
else 
ifdef NCURSES 
  LDFLAGS+= -lncurses -lpanel -lm
else
  LDFLAGS+= pdc33_lin_x86/libXCurses.a -lm -lXaw -lXmu -lXt -lX11 -lSM -lICE -lXext -lXpm -lc
  
  CFLAGS+= -D__PDC__ -D__WIN32__
  MDPORT_FLAGS=-D__PDC__
endif
  BIN= $@
endif

cr: main.c map.o utils.o entities.o items.o io.o mdport.o
	${CC} ${CFLAGS} $? -o ${BIN} ${LDFLAGS}
	strip ${BIN}
	rm -f *.o

mdport.o: mdport.c
	${CC} -c ${MDPORT_FLAGS} -o $@ $? 

astyle:
	astyle --style=java -t -n *.c *.h
ctags:
	ctags -f ~/tags $(shell pwd)/*.c $(shell pwd)/*.h
clean:
	rm -f *.o *~ cr cr.exe
