default: help

##@ Helpers
.PHONY: help
help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

KNOWN_TARGETS := $(shell awk '/^[a-zA-Z_-]+:/ { print $$1 }' $(MAKEFILE_LIST) | sed 's/://')
ARGS := $(filter-out $(KNOWN_TARGETS),$(MAKECMDGOALS))
.DEFAULT: ;: do nothing

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

SOURCES = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/main

# Default target
all: $(TARGET)

##@ Build

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build the executable from object files
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(SFML_FLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build: $(TARGET)  ## Build the project (incremental)

clean:  ## Clean object files
	rm -rf $(OBJ_DIR)

fclean: clean  ## Clean everything
	rm -f $(TARGET)

re: fclean build ## Rebuild the project from scratch

# Include dependency files if they exist
-include $(OBJECTS:.o=.d)

# Generate dependency files
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
