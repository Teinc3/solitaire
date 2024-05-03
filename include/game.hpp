#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "card.hpp"
#include "board.hpp"
#include "logic.hpp"

class Display; // Forward declaration

class Game
{
public:
    Game();
    ~Game();

    void createGame();
    void cleanUp();

    void createCards();
    void deleteCards();
    void shuffleCards();

    void update();

    void handleInput();

    bool getIsRunning();
    Display* getDisplay();
    GameState getGameState();
    MenuOption getMenuOption();

private:
    bool isRunning;
    GameState gameState;
    MenuOption menuOption;

    Card* deck[MAX_CARDS];
    
    Board* board;
    Logic* logic;
    Display* display;
};