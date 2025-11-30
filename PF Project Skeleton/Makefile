# ============================================================================
# MAKEFILE - Build configuration for Switchback Rails
# ============================================================================
# This Makefile compiles all C++ source files and links them with SFML
# to create the executable game.
#
# USAGE:
#   make           - Compile the project
#   make clean     - Remove all compiled files
#   make run       - Compile and run with default level
# ============================================================================

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# SFML libraries to link
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Directories
CORE_DIR = core
SFML_DIR = sfml
OUT_DIR = out
DATA_DIR = data/levels

# Source files
CORE_SOURCES = $(CORE_DIR)/simulation_state.cpp \
               $(CORE_DIR)/simulation.cpp \
               $(CORE_DIR)/trains.cpp \
               $(CORE_DIR)/switches.cpp \
               $(CORE_DIR)/grid.cpp \
               $(CORE_DIR)/io.cpp

SFML_SOURCES = $(SFML_DIR)/app.cpp \
               $(SFML_DIR)/main.cpp

# All source files
SOURCES = $(CORE_SOURCES) $(SFML_SOURCES)

# Object files (replace .cpp with .o)
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
TARGET = switchback_rails

# Default level file
DEFAULT_LEVEL = $(DATA_DIR)/easy_level.lvl

# ============================================================================
# BUILD RULES
# ============================================================================

# Default target - build the executable
all: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "Build complete!"
	@echo "=========================================="
	@echo "Run the game with:"
	@echo "  ./$(TARGET) $(DEFAULT_LEVEL)"
	@echo ""

# Link object files to create executable
$(TARGET): $(OBJECTS)
	@echo ""
	@echo "Linking executable..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Executable created: $(TARGET)"

# Compile each .cpp file to .o object file
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# UTILITY TARGETS
# ============================================================================

# Clean all compiled files
clean:
	@echo "Cleaning build files..."
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f $(OUT_DIR)/*.csv
	rm -f $(OUT_DIR)/*.txt
	@echo "Clean complete!"

# Run the game with default level
run: $(TARGET)
	@echo ""
	@echo "Running game with default level..."
	@echo ""
	./$(TARGET) $(DEFAULT_LEVEL)

# Run with easy level
easy: $(TARGET)
	@echo "Running easy level..."
	./$(TARGET) $(DATA_DIR)/easy_level.lvl

# Run with medium level
medium: $(TARGET)
	@echo "Running medium level..."
	./$(TARGET) $(DATA_DIR)/medium_level.lvl

# Run with hard level
hard: $(TARGET)
	@echo "Running hard level..."
	./$(TARGET) $(DATA_DIR)/hard_level.lvl

# Run with complex network level
complex: $(TARGET)
	@echo "Running complex network level..."
	./$(TARGET) $(DATA_DIR)/complex_network.lvl

# Create output directory if it doesn't exist
setup:
	@echo "Setting up directories..."
	mkdir -p $(OUT_DIR)
	@echo "Setup complete!"

# Show help
help:
	@echo "=========================================="
	@echo "Switchback Rails - Makefile Help"
	@echo "=========================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make           - Build the project"
	@echo "  make clean     - Remove compiled files"
	@echo "  make run       - Build and run default level"
	@echo "  make easy      - Run easy level"
	@echo "  make medium    - Run medium level"
	@echo "  make hard      - Run hard level"
	@echo "  make complex   - Run complex network level"
	@echo "  make setup     - Create output directories"
	@echo "  make help      - Show this help message"
	@echo ""
	@echo "Example:"
	@echo "  make clean && make && make run"
	@echo ""

# ============================================================================
# PHONY TARGETS
# ============================================================================
# These targets don't represent actual files
.PHONY: all clean run easy medium hard complex setup help