# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++11
INCLUDES = -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, bin/o/%.o, $(SRC))
TARGET = bin/solitaire

# Default target
all: clean $(TARGET)

# Create the directories
bin/o/:
	mkdir -p bin/o

bin/:
	mkdir -p bin

# Linking
$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(INCLUDES)

# Compilation
bin/o/%.o: src/%.cpp | bin/o/
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

# Clean up
clean:
	rm -rf bin

.PHONY: all clean