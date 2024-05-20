#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <curses.h>
    #define NCURSES 0
    
    enum ArrowKey
    {
        UP = 450,
        LEFT = 452,
        RIGHT = 454,
        DOWN = 456
    };
#else
    #include <ncurses.h>
    #define NCURSES 1

    enum ArrowKey
    {
        DOWN = 258,
        UP,
        LEFT,
        RIGHT
    };
#endif

using std::string;
using std::vector;

constexpr int MAX_CARDS = 52;
constexpr int SUIT_COUNT = 4;
constexpr int MAX_VALUE = 13;

constexpr int STACK_COUNT = 7;
constexpr int FOUNDATION_COUNT = 4;
constexpr int RESERVED_CARDS = 24;

constexpr int COL_COUNT = 1 + STACK_COUNT + 2;

constexpr int HEIGHT = 22; //1+1+(3+13+1)+1+1+1
constexpr int MIN_WIDTH = 76; //MIN_WIDTH+(2+5)
constexpr int COL_WIDTH = 7;

constexpr int HORIZ_CURSOR_XPOS[10] = { 2, 10, 17, 24, 31, 38, 45, 52, 60, 67 };

constexpr int EMPTY_PILE_CURSORPIILE_YHEIGHT = HEIGHT - 5;

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

void coloredPrint(int, int, int, string);
string formatString(string, size_t, string[]);

enum Suit
{
    DIAMONDS,
    CLUBS,
    HEARTS,
    SPADES
};

enum MenuOption
{
    NEW_GAME = 1,
    LOAD_SAVE_GAME,
    INFO,
    QUIT
};

enum GameState
{
    MAIN_MENU,
    PLAYING,
    GAME_MENU
};

struct Card
{
    Suit suit;
    int value;
    bool isFaceUp;
};

bool isRed(Suit);