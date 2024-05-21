#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <curses.h>
    #define WINDOWS 1
#else
    #include <ncurses.h>
    #define WINDOWS 0
#endif

enum PSArrowKey
{
    _UP = 450,
    _LEFT = 452,
    _RIGHT = 454,
    _DOWN = 456
};

enum ArrowKey
{
    DOWN = 258,
    UP,
    LEFT,
    RIGHT
};

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

constexpr int COLORPAIR_COUNT = 7;

enum ColorPair
{
    WHITE = 0, // Unused
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA
};

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
    GAME_MENU,
    INFO_PAGE,
    PLAYING
};

struct Card
{
    Suit suit;
    int value;
    bool isFaceUp;
};

struct ColorRange
{
    ColorPair color;
    // Exclusive
    int end;
};

// Prints text in a single color
void monoColorPrint(ColorPair, int, int, string);
// Prints text in multiple colors
void multiColorPrint(int, int, string, int, ColorRange*);
string formatString(string, size_t, string[]);
bool isRed(Suit);