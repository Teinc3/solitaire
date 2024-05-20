#pragma once

#include "common.hpp"

#include "board.hpp"

struct CursorPileInfo
{
    int startingY = 2; // For Foundation piles

    /*
    Unused pile: At unusedCardIndex = 0 is False (Then, show X + Last unused card), otherwise True
    Stack: Depends
    Foundation: Always False
    */
    bool hasHiddenCard = false;
    
    /*
    Unused Pile: 0 or 1
    Stacks: 0 to visibleCardCount + (hiddenCardCount > 0 ? 1 : 0)
    Foundation: 0 to 1 for both cols
    If you have hiddenCard you will most likely bump the y Coordinate up by 2
    */
    int currentCursorVerticalIndex = 0;
    int yHeight = 3;
    // For unused Pile, At MAX value (24) = ? + No display Cards, 0 = X + Last displayed card, basically -1 to 23 MAX
};

class Cursor
{
public:
    Cursor(Board*);

    void onNewGame();

    void render();
    void clampCursorPiles();

    void updateHorizCursorX(bool);
    void updateVerticalCursorIndex(bool);
    void updateCursorLock(bool);

    int getHorizCursorXIndex();
    int getVerticalCursorIndex();
    int getLockedCursorPileIndex();

private:
    Board* board = nullptr;

    int horizCursorXIndex;
    // If true, then when we change horizCursorXIndex, we do not also change the stackIndex
    bool isLockedCursor;
    // -1 if horizcursor is not on stacks, otherwise 0 to 6. 
    int lockedCursorPileIndex;
    // Also an array that keeps track of heights of each stack
    CursorPileInfo pileCursors[COL_COUNT];

};