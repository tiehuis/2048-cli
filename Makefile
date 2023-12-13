CC             ?= clang
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

remake: clean all

clean:
	rm -f $(PROGRAM)

# GoogleTest Setup
GTEST_DIR := googletest-main
GTEST_SRCS := $(wildcard $(GTEST_DIR)/src/*.cc) $(wildcard $(GTEST_DIR)/src/*.h)

# Test Files
TEST_SRCS := $(wildcard test/*.cpp)

# Test Compiler Flags
# Update the include path to point to GoogleTest's include directory
CPPFLAGS += -isystem $(GTEST_DIR)/googletest/include -Isrc
CXXFLAGS += -g -Wall -Wextra -pthread

# Test Targets
test: $(TEST_SRCS) $(GTEST_SRCS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(TEST_SRCS) src/ai.c src/engine.c src/highscore.c src/options.c -o test_program $(LDFLAGS) -lpthread -lgtest

clean-test:
	rm -f test_program

.PHONY: test clean-test


