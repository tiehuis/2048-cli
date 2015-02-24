CC      ?= clang
CFLAGS  += -g -Wall -Wextra
LFLAGS  +=
DEFINES := -DINVERT_COLORS -DVT100 $(shell pkg-config --cflags sdl2)

PROGRAM := 2048
C_FILES := $(wildcard src/*.c)
O_FILES := $(addprefix obj/, $(notdir $(C_FILES:.c=.o)))
FILTERED_O_FILES := $(filter-out obj/gfx%.o obj/merge%.o, $(O_FILES))

all: curses

curses: $(O_FILES)
	$(CC) $(FILTERED_O_FILES) obj/merge_std.o obj/gfx_curses.o -o $(PROGRAM) -lcurses

vt100: $(O_FILES)
	$(CC) $(FILTERED_O_FILES) obj/merge_std.o obj/gfx_terminal.o -o $(PROGRAM)

sdl: $(O_FILES)
	$(CC) $(FILTERED_O_FILES) obj/merge_std.o obj/gfx_sdl.o -o $(PROGRAM) -lSDL2 -lSDL2_ttf

obj/%.o: src/%.c
	$(CC) $(DEFINES) $(CFLAGS) -c -o $@ $<

remake: clean all

clean:
	rm -f $(O_FILES) $(PROGRAM)

.PHONY: clean remake
