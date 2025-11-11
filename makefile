PROGRAM := pong
CC := clang
CFLAGS := -Wall -Wextra -O3 -std=c23 -ggdb
LDFLAGS := -lncurses -lm -fsanitize=address,undefined

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))
BIN := bin/$(PROGRAM)

all: $(BIN)

dev: format
	bear -- make $(BIN)

# Main program
$(BIN): $(OBJ) | bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Dirs
build:
	@mkdir -p build

bin:
	@mkdir -p bin

# Extra
clean:
	rm -rf build bin compile_commands.json

format:
	@find src/ -iname "*.h" -o -iname "*.c" | xargs clang-format -i

run: all
	@./$(BIN)

.PHONY: all check clean format run
