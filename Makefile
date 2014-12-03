CC	   ?= gcc
CFLAGS += -Wall -Wextra
LIBS	= -lcurses

all: 2048

2048: src/2048.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) src/2048.c -o 2048

2048nc: src/2048.c
	$(CC) -DNO_CURSES=1 $(CFLAGS) $(LDFLAGS) src/2048.c -o 2048nc

clean:
	rm -f 2048 2048nc
