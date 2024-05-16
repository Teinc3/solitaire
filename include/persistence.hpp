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

    char* writeSaveData();

    void writeStackData(int, char*, int*);
    void writeFoundationData(int, char*, int*);
    void writeUnusedData(char*, int*);
    void writeSep(char*, int*);
    void writeCardData(Card*, char*, int*);

    int getArrayLength();
};