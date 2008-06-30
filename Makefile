-include config
VPATH= src:../headers

ifdef DBG
CFLAGS+= -std=c99 -O0 -g -Wall -Wextra -pedantic
else
CFLAGS+= -std=c99 -Os
endif

STRIP= strip
BIN= $@
ifdef CROSS
  #cross compiling to windows
  WINDIR=1
  CC= i586-mingw32msvc-gcc
  STRIP= i586-mingw32msvc-strip
endif
ifdef WINDIR
  #windows+pdcurses
  LDFLAGS+= lib/libpdcurses.a -lm 
  CFLAGS+= -D__PDC__ -D__WIN32__
  BIN= $@.exe
  MDPORT_FLAGS=-D__PDC__ -D__WIN32__
ifeq (${SDL}, 1)
ifndef CROSS
    #windows native sdl
    CFLAGS+= $(shell sdl-config --cflags) -D__SDL__
    LDFLAGS+= $(shell sdl-config --libs) -lSDL_mixer
endif
endif
else ifeq ($(shell uname), Linux)
ifeq (${MODE}, PDC)
    #linux+pdcurses
    LDFLAGS+= lib/libXCurses.a -lm -lXaw -lXpm
    CFLAGS+= -D__PDC__
    MDPORT_FLAGS=-D__PDC__
else
    #linux+ncurses
    LDFLAGS+= -lncurses -lpanel -lm
endif
ifeq (${SDL}, 1)
    #linux sdl
    CFLAGS+= $(shell sdl-config --cflags) -D__SDL__
    LDFLAGS+= $(shell sdl-config --libs) -lSDL_mixer
endif
else
  #all others+ncurses
  LDFLAGS+= -lncurses -lpanel -lm
endif

cr: main.c map.o utils.o entities.o items.o io.o mdport.o
	${CC} ${CFLAGS} $? -o ${BIN} ${LDFLAGS}
ifndef DBG
	${STRIP} ${BIN}
endif
	rm -f *.o

mdport.o: mdport.c
	${CC} -c ${MDPORT_FLAGS} -o $@ $? 

astyle:
	astyle --style=java -t -n src/*.c src/*.h lib/*.h

clean:
	rm -f *.o *~ cr cr.exe
