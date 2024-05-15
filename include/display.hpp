#pragma once

#include "common.hpp"
#include "card.hpp"
#include "game.hpp"
#include "cursor.hpp"

constexpr int MIN_WIDTH = 69; //1+1+5+2+1+(2+5)*7+2+1+2+5+1+1
constexpr int HEIGHT = 22; //1+1+(3+13+1)+1+1+1
constexpr int MIN_2COL_FOUNDATION_WIDTH = 76; //MIN_WIDTH+(2+5)
constexpr int COL_WIDTH = 7;

constexpr int MSG_STARTING_X = 2;
constexpr int MOVE_MSG_STARTING_X = 62;
constexpr int MAX_MSG_LENGTH = 56;
constexpr const char* MESSAGES[] = {
    "", // No message
    "Congratulations! You won!", // Yellow
    " (Backspace to Dismiss)",
    "Auto Finish (Enter to Confirm)", // Yellow
    "Invalid move", // Red
    // Add more invalid moves here, so index will not fuck up
};

class Display {
public:
    Display(Game*);
    ~Display();

    void onNewGame();

    void render();

    int is2ColFoundation();

    Cursor* getCursor();

    void setMessage(int);
    bool resetMessage(bool);

private:
    int width;
    bool use2ColFoundation;

    int currentMessageIndex;

    Cursor* cursor = nullptr;
    Game* game = nullptr;

    void drawBoundary();
    void drawMenu(bool);
    void drawGameBoard();

    // Secondary drawing functions
    void drawDelimiter(int);
    void drawUnusedPile();
    void drawStack(int);
    void drawFoundation(Suit);
    void drawMessage();

    int drawCard(int, int, int, int, Card*[]);
    void drawCardDivider(int, int, bool);

    string getSuitChar(Suit);
    string getValueChar(int);
};