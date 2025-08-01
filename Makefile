.PHONY: clean

CC := gcc
CFLAGS := -I./include -Wall -Wextra -Wpedantic -O3

BUILD_DIR := build
BIN_DIR := bin

all: $(BUILD_DIR) $(BIN_DIR) bin/encode bin/search

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

bin/encode: src/encode.c build/radix_tree.o build/radix_tree_file_writer.o
	$(CC) $(CFLAGS) -o bin/encode src/encode.c build/radix_tree.o build/radix_tree_file_writer.o

bin/search: src/search.c build/radix_tree_file_reader.o
	$(CC) $(CFLAGS) -o bin/search src/search.c build/radix_tree_file_reader.o

build/radix_tree_file_reader.o: include/radix_tree_file_reader.h src/radix_tree_file_reader.c
	$(CC) $(CFLAGS) -c src/radix_tree_file_reader.c -o build/radix_tree_file_reader.o

build/radix_tree_file_writer.o: include/radix_tree_file_writer.h src/radix_tree_file_writer.c
	$(CC) $(CFLAGS) -c src/radix_tree_file_writer.c -o build/radix_tree_file_writer.o

build/radix_tree.o: include/radix_tree.h src/radix_tree.c
	$(CC) $(CFLAGS) -c src/radix_tree.c -o build/radix_tree.o

clean:
	rm -rf build
	rm -rf bin
