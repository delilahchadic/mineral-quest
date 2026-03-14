# Compiler and Flags
CC = gcc
# Added -Isrc so you can include headers like #include "core/common.h"
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -Isrc
LDFLAGS = -lraylib 

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 1. RECURSIVE SEARCH: This finds all .c files in src and all subfolders
SRC = $(shell find $(SRC_DIR) -name "*.c")

# 2. OBJECT MAPPING: This mirrors the src subfolder structure inside obj/
# e.g., src/world/map.c becomes obj/world/map.o
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/game

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# 3. COMPILATION: This creates subfolders in obj/ on the fly
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the game
run: all
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean run