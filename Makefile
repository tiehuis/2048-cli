CC	   ?= gcc
CFLAGS += -Wall -Wextra
LIBS	= -lcurses

all: 2048

2048: src/2048.c src/2048.h src/highscore_file.c
	$(CC) $(CFLAGS) src/2048.c src/highscore_file.c -o 2048 $(LIBS)

2048nc: src/2048_no_curses.c src/2048.h src/highscore_file.c
	$(CC) $(CFLAGS) src/2048_no_curses.c src/highscore_file.c -o 2048nc

clean:
	rm -f 2048 2048nc
