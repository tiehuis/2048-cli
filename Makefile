ASCIIDOC ?= a2x
CC       ?= gcc
CFLAGS   := -Wall -Wextra $(CFLAGS)
LIBS      = -lcurses

.PHONY: clean man man-2048 man-2048nc

2048: src/2048.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) src/2048.c -o 2048

2048nc: src/2048.c
	$(CC) -DNO_CURSES=1 $(CFLAGS) $(LDFLAGS) src/2048.c -o 2048nc

all: 2048

clean:
	rm -f 2048 2048nc

man: man-2048 man-2048nc

man-2048:
	rm -f man/2048.1
	$(ASCIIDOC) -d manpage -f manpage man/2048.1.txt

man-2048nc:
	rm -f man/2048nc.1
	$(ASCIIDOC) -d manpage -f manpage man/2048nc.1.txt
