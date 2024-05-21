#pragma once

#include "common.hpp"
#include "board.hpp"

#define SEP_COUNT 9

constexpr char SAVEFILE_NAME[] = "save.sol";

class Persistence
{
public:
    Persistence(Board* board, Card** deck);
    ~Persistence();

    bool saveFile();
    bool loadFile();
    void saveDebugInfo(string);

private:
    Board* board = nullptr;
    Card** deck = nullptr;

    int getArrayLength();

    char* writeSaveData(int);
    bool readSaveData(char*, int);

    void writeStackData(int, char*, int*);
    void writeFoundationData(int, char*, int*);
    void writeUnusedData(char*, int*);
    void writeSep(char*, int*);
    void writeCardData(Card*, char*, int*);

    bool readStackData(char*, int, int, int);
    bool readFoundationData(char*, int, int);
    bool readUnusedData(char*, int, int);
    bool readMovesData(char*, int);
    Card* readCardData(char);

};