#pragma once

#include "common.hpp"
#include "card.hpp"
#include "game.hpp"

#define GREEN 1
#define RED 2
#define BLACK 3
#define YELLOW 4
#define BLUE 5

/*
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
*/

constexpr int MIN_WIDTH = 69; //1+1+5+2+1+(2+5)*7+2+1+2+5+1+1
constexpr int HEIGHT = 21; //1+1+(3+13+1)+1+1
constexpr int MIN_2COL_FOUNDATION_WIDTH = 76; //MIN_WIDTH+(2+5)
constexpr int COL_WIDTH = 7;
constexpr int HORIZ_CURSOR_XPOS[10] = { 2, 10, 17, 24, 31, 38, 45, 52, 60, 67 };

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
    Foundation: If use2ColFoundation, 0 or 1, otherwise 0 to 3
    If you have hiddenCard you will most likely bump the y Coordinate up by 2
    */
    int currentCursorVerticalIndex = 0;
    int yHeight = 3;
    // For unused Pile, At MAX value (24) = ? + No display Cards, 0 = X + Last displayed card, basically -1 to 23 MAX
};

class Display {
public:
    Display(Game*);
    ~Display();

    void render();

    void updateHorizCursorX(bool);
    void clampCursorPiles();
    void updateVerticalCursorIndex(bool);
    void updateCursorLock(bool);

    int getHorizCursorXIndex();
    int getVerticalCursorIndex();
    int getLockedCursorPileIndex();
    int is2ColFoundation();

private:
    int width;
    bool use2ColFoundation;

    int horizCursorXIndex;
    // If true, then when we change horizCursorXIndex, we do not also change the stackIndex
    bool isLockedCursor;
    // -1 if horizcursor is not on stacks, otherwise 0 to 6. 
    int lockedCursorPileIndex;
    // Also an array that keeps track of heights of each stack
    CursorPileInfo pileCursors[1 + STACK_COUNT + 2];

    Game* game = nullptr;

    void drawBoundary();
    void drawMenu(bool);
    void drawGameBoard();

    // Secondary drawing functions
    void drawDelimiter(int);
    void drawUnusedPile();
    void drawStack(int);
    void drawFoundation(Suit);

    void drawCursor();
    int drawCard(int, int, int, int, Card*[]);
    void drawCardDivider(int, int, bool);

    string getSuitChar(Suit);
    string getValueChar(int);
};