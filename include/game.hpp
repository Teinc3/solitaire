#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "card.hpp"
#include "board.hpp"
#include "logic.hpp"

class Game
{
public:
    Game();
    ~Game();

    void gameInit();
    void cleanUp();

    void createCards();
    void deleteCards();
    void shuffleCards();

    void update();

    bool getIsRunning();

private:
    bool isRunning;

    Card* deck[MAX_CARDS];
    
    Board* board;
    Logic* logic;
};