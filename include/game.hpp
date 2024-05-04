#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "card.hpp"
#include "logic.hpp"

// Forward declarations
class Display;
class Board;

class Game
{
public:
    Game();
    ~Game();

    void createGame();
    void cleanUp(bool);

    void createCards();
    void deleteCards();
    void shuffleCards();

    void update();

    void handleInput();

    bool getIsRunning();
    GameState getGameState();
    MenuOption getMenuOption();

    Display* getDisplay();
    Board* getBoard();
    
    void setIsRunning(bool);

private:
    bool isRunning;
    bool isGamePreviouslyCreated;
    GameState gameState;
    MenuOption menuOption;

    Card* deck[MAX_CARDS];
    
    Board* board;
    Logic* logic;
    Display* display;

    void handleArrowKeys(ArrowKey);
    void handleEnterKey();
};