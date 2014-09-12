CC	   ?= gcc
CFLAGS += -Wall -Wextra
LIBS	= -lcurses

all: 2048

2048: src/2048.c
	$(CC) $(CFLAGS) src/2048.c -o 2048 $(LIBS)

2048nc: src/2048_no_curses.c
	$(CC) $(CFLAGS) src/2048_no_curses.c -o 2048nc

clean:
	rm -f 2048 2048nc
