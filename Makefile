CC	    := clang
CFLAGS  += -g -Wall -Wextra
LFLAGS  +=
DEFINES := -DVT100 -D_REENTRANT -I/usr/include/SDL2

PROGRAM := 2048
C_FILES := $(wildcard src/*.c)
O_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

all: curses

curses: $(O_FILES)
	$(CC) $(filter-out obj/gfx%.o, $(O_FILES)) obj/gfx_curses.o -o $(PROGRAM) -lcurses

vt100: $(O_FILES)
	$(CC) $(filter-out obj/gfx%.o, $(O_FILES)) obj/gfx_terminal.o -o $(PROGRAM)

sdl: $(O_FILES)
	$(CC) $(filter-out obj/gfx%.o, $(O_FILES)) obj/gfx_sdl.o -o $(PROGRAM) -lSDL2 -lSDL2_ttf

obj/%.o: src/%.c
	$(CC) $(DEFINES) $(CFLAGS) -c -o $@ $<

remake: clean all

clean:
	rm -f obj/*
	rm -f 2048

.PHONY: clean remake
