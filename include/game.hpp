#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "card.hpp"
#include "board.hpp"

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

    Card* (*getDeck())[MAX_CARDS];
    bool getIsRunning();
private:
    Board* board;
    Card* deck[MAX_CARDS];
    bool isRunning;
};