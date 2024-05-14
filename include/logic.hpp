#pragma once

#include "common.hpp"

#include "board.hpp"
#include "display.hpp"

class Board;
class Display;

class Logic
{
public:
    Logic(Board*, Display*);
    ~Logic();

    bool isGameWon();
    bool canAutoFinish();

    void handleUnusedCardSelection(int);
    bool handleStackSelection(int, int, int);
    bool handleFoundationSelection(int, int);

private:
    Board* board = nullptr;
    Display* display = nullptr;

    bool stackToStack(int, int, int);
    bool stackToFoundation(int);
    bool unusedToStack(int);
    bool unusedToFoundation();
    bool foundationToStack(int, int);

    static bool canExistingStackAcceptCard(Card*, Card*);
    static bool canEmptyStackAcceptCard(Card*);

    static bool canEmptyFoundationAcceptCard(Card*);
    static bool canExistingFoundationAcceptCard(Card*, Card*);
};