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

    void handleUnusedCardSelection(int);

    bool stackToStack(int, int, int);
    bool stackToFoundation(int);
    bool unusedToStack(int);
    bool unusedToFoundation();
    bool foundationToStack(int, int);

private:
    Board* board;
    Display* display;

    static bool canExistingStackAcceptCard(Card*, Card*);
    static bool canEmptyStackAcceptCard(Card*);

    static bool canEmptyFoundationAcceptCard(Card*);
    static bool canExistingFoundationAcceptCard(Card*, Card*);
};