#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "card.hpp"

// Forward declarations

class Display;
class Board;
class Logic;

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

    Card* deck[MAX_CARDS] = { nullptr };
    
    Board* board = nullptr;
    Logic* logic = nullptr;
    Display* display = nullptr;

    void handleArrowKeys(ArrowKey);
    void handleEnterKey();
};