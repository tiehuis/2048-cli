CC?=gcc
CFLAGS+=

all: 2048 2048_no_curses

2048: 2048_curses.c
	$(CC) $(CFLAGS) 2048_curses.c -lcurses -o 2048

2048_no_curses: 2048_no_curses.c
	$(CC) $(CFLAGS) 2048_no_curses.c -o 2048_no_curses

clean:
	rm -f 2048 2048_no_curses
