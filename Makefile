# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -O2 -I$(SRC_DIR)/include -I$(TEST_DIR)/include -I$(TEST_DIR)/simple_mapping/include

# Project structure
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRCS))
TEST_OBJS += $(patsubst $(TEST_DIR)/simple_mapping/%.c, $(OBJ_DIR)/%.o, $(TEST_SRCS))

# Output executables
TARGET = $(BIN_DIR)/simulator
TEST_TARGET = $(BIN_DIR)/test_simulator

# Default target
all: $(TARGET)

# Rule to build the simulator target
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build test target
$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build object files from src
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build object files from test
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean up build artifacts
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean test
