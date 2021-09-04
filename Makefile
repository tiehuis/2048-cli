CC             ?= clang
CFLAGS         += -Wall -Wextra
CFLAGS         += -DINVERT_COLORS -DVT100 -O2
LFLAGS         +=

PROGRAM := 2048
C_FILES := $(wildcard src/*.c)
MERGE_FILE := src/merge_std.c
#MERGE_FILE := src/merge_fib.c
FILTERED_C_FILES := $(filter-out src/gfx%.c src/merge%.c, $(C_FILES))

#all: terminal
all: curses

curses: $(FILTERED_C_FILES) src/gfx_curses.c
	$(CC) $(CFLAGS) -DNCURSES $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_curses.c -o $(PROGRAM) $(LFLAGS) -lcurses

terminal: $(FILTERED_C_FILES) src/gfx_terminal.c
	$(CC) $(CFLAGS) $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_terminal.c -o $(PROGRAM) $(LDFLAGS)

remake: clean all

clean:
	rm -f $(PROGRAM)

.PHONY: clean remake
