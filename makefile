PROGRAM := pong
CC := clang
CFLAGS := -Wall -Wextra -Iinclude -Ilib/Unity/src -O0 -ggdb -std=c99 -lncurses -fsanitize=address

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))
BIN := bin/$(PROGRAM)

all: format
	bear -- make $(BIN)

# Main program
$(BIN): $(OBJ) | bin
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Dirs
build:
	@mkdir -p build

bin:
	@mkdir -p bin

# Extra
clean:
	rm -rf build bin compile_commands.json test_build

format:
	@find src/ -iname "*.h" -o -iname "*.c" | xargs clang-format -i

run: all
	@./$(BIN)

.PHONY: all check clean format run
