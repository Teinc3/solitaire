#pragma once

#include "common.hpp"
#include "board.hpp"

#define LINE_BREAK_COUNT 9

constexpr char SAVEFILE_NAME[] = "save.sol";

class Persistence
{
public:
    Persistence(Board* board);
    ~Persistence();

    bool saveFile();
    bool loadFile();

private:
    Board* board = nullptr;

    char* generateSaveData();

    void generateStackData(int, char*, int*);
    void generateFoundationData(int, char*, int*);
    void generateUnusedData(char*, int*);
    void generateNewLine(char*, int*);
    void writeCardData(Card*, char*, int*);

    int getArrayLength();
};