# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++11 -O2 # Remove -g and add -O2 when release
INCLUDES = -Iinclude

SRC = $(wildcard src/*.cpp)

ifeq ($(OS),Windows_NT)
	OBJ = $(patsubst src/%.cpp, bin/o/%.obj, $(SRC))
	TARGET = bin\solitaire.exe
	LIBS = -lpdcurses
	MAKEDIR = mkdir bin\o
else
	DIR_SEP = /
	OBJ = $(patsubst src/%.cpp, bin/o/%.o, $(SRC))
	TARGET = bin/solitaire
	LIBS = -lncurses
	MAKEDIR = mkdir -p bin/o
endif

# Default target
all: clean build

# Create the directories
bin/o/:
	$(MAKEDIR)

# Compilation
bin/o/%.o bin/o/%.obj: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

# Linking
$(TARGET): bin/o/ | $(OBJ)
	$(CXX) $(OBJ) -o $@ $(INCLUDES) $(LIBS)

# Clean up
clean:
ifeq ($(OS),Windows_NT)
	if exist bin rmdir /s /q bin
else
	rm -rf bin
endif

# Build
build: $(TARGET)

.PHONY: all clean build

print:
	@echo $(OS)
	@echo $(SRC)
	@echo $(OBJ)
	@echo $(TARGET)
	@echo $(LIBS)
	@echo $(MAKEDIR)