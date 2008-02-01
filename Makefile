'CFLAGS= -std=c99 -O0 -g -Wall -Wextra -pedantic
CFLAGS= -std=c99 -O2 
LDFLAGS= -lcurses -lm

all: $(basename $(wildcard *.c)) 

astyle:
	astyle --style=java -t -n *.c
ctags:
	ctags -f ~/tags $(shell pwd)/*.c
