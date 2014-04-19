CC=gcc
CCARGS=

all: 2048 2048_no_curses

2048: 2048_curses.c
	$(CC) $(CCARGS) 2048_curses.c -lcurses -o 2048

2048_no_curses: 2048_no_curses.c
	gcc 2048_no_curses.c -o 2048_no_curses
