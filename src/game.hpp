#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "card.hpp"

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

    Card* (*getDeck())[52];
    bool getIsRunning();
private:
    Card* deck[52];
    bool isRunning;
};