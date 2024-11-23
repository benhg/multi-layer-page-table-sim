# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -g -MMD

# Directories
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = build

# Include directories (recursively)
INCLUDE_DIRS := $(shell find $(SRC_DIR) $(TEST_DIR) -type d -name include)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))

# Source files
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
TEST_FILES := $(shell find $(TEST_DIR) -name '*.c')
ALL_FILES := $(SRC_FILES) $(TEST_FILES)

# Object files
OBJ_FILES := $(patsubst %.c, $(OBJ_DIR)/%.o, $(ALL_FILES))

# Dependency files
DEP_FILES := $(OBJ_FILES:.o=.d)

# Target executable
TARGET = simulator

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ_FILES)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ $^

# Compile object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Clean up build files
clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJ_DIR) $(TARGET) $(DEP_FILES)

# Include dependency files if they exist
-include $(DEP_FILES)