CC             ?= clang
TTF_FONT_PATH  ?= res/Anonymous Pro.ttf
CFLAGS         += -DTTF_FONT_PATH="\"$(TTF_FONT_PATH)\""
CFLAGS         += -Wno-visibility -Wno-incompatible-pointer-types -Wall -Wextra
CFLAGS         += -DINVERT_COLORS -DVT100 -O2
LFLAGS         +=

PROGRAM := 2048
C_FILES := $(wildcard src/*.c)
MERGE_FILE := src/merge_std.c
FILTERED_C_FILES := $(filter-out src/gfx%.c src/merge%.c, $(C_FILES))

all: terminal

curses: $(FILTERED_C_FILES) src/gfx_curses.c
	$(CC) $(CFLAGS) $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_curses.c -o $(PROGRAM) $(LDFLAGS) -lcurses

terminal: $(FILTERED_C_FILES) src/gfx_terminal.c
	$(CC) $(CFLAGS) $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_terminal.c -o $(PROGRAM) $(LDFLAGS)

sdl: $(FILTERED_C_FILES) src/gfx_sdl.c
	$(CC) $(CFLAGS) $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_sdl.c -o $(PROGRAM) $(shell pkg-config --cflags sdl2) $(LDFLAGS) -lSDL2 -lSDL2_ttf

remake: clean all

clean:
	rm -f $(PROGRAM)

.PHONY: clean remake
