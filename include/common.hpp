#pragma once

#include <vector>
#include <iostream>
#include <string>

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