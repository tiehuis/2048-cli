CC	   ?= gcc
CFLAGS += -Wall -Wextra
DEFS	= -DVT100_COMPATIBLE
LIBS	= 

all: 2048

2048: src/2048_engine.c src/2048_rewrite.c
	$(CC) $(DEFS) src/2048_engine.c src/2048_rewrite.c -o 2048 $(LIBS)
	
clean:
	rm -f 2048

.PHONY: clean
