#pragma once

#include "common.hpp"
#include "board.hpp"

class Logic
{
public:
    Logic(Board*);

    bool stackToStack(int, int, int);
    bool stackToFoundation(int);
    bool unusedToStack(int);
    bool unusedToFoundation();
    bool foundationToStack(int, int);

private:
    Board* board;

    static bool canExistingStackAcceptCard(Card*, Card*);
    static bool canEmptyStackAcceptCard(Card*);

    static bool canEmptyFoundationAcceptCard(Card*);
    static bool canExistingFoundationAcceptCard(Card*, Card*);
};