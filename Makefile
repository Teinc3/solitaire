# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++11
INCLUDES = -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, bin/o/%.o, $(SRC))
TARGET = bin/solitaire

# Default target
all: $(TARGET)

# Create the directories
bin/o/:
	mkdir -p bin/o

bin/:
	mkdir -p bin

# Compilation
bin/o/%.o: src/%.cpp | bin/o/
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

# Linking
$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(INCLUDES)

# Clean up
clean:
	rm -rf bin

.PHONY: all clean