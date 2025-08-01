# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = chip8_emulator

.PHONY: all clean run

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Run with a default ROM for convenience
run: all
	./$(EXECUTABLE) roms/PONG

clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE)