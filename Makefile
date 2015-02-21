CC	    := clang
CFLAGS  += -O2 -Wall -Wextra
LFLAGS  +=
DEFINES := -DVT100

PROGRAM := 2048
C_FILES := $(wildcard src/*.c)
O_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

all: curses

curses: $(O_FILES)
	$(CC) $(filter-out obj/gfx%.o, $(O_FILES)) obj/gfx_curses.o -o $(PROGRAM) -lcurses

vt100: $(O_FILES)
	$(CC) $(filter-out obj/gfx%.o, $(O_FILES)) obj/gfx_terminal.o -o $(PROGRAM)

obj/%.o: src/%.c
	$(CC) $(DEFINES) $(CFLAGS) -c -o $@ $<

remake: clean all

clean:
	rm -f obj/*
	rm -f 2048

.PHONY: clean remake
