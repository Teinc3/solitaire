#pragma once

#include <random>
#include <ctime>

#include "common.hpp"
#include "persistence.hpp"

// Forward declarations

class Display;
class Board;
class Logic;

class Game
{
public:
    Game();
    ~Game();

    void createGame(bool);
    void finishGame();

    void update();

    void handleInput();

    bool getIsRunning();
    bool getHasAlreadyWon();
    GameState getGameState();
    MenuOption getMenuOption();

    Display* getDisplay();
    Board* getBoard();
    
    void setIsRunning(bool);

private:
    bool isRunning;
    bool isGamePreviouslyCreated;
    bool hasAlreadyWon;
    bool hasAlreadyPromptedAutoFinished;
    GameState gameState;
    MenuOption menuOption;

    Card* deck[MAX_CARDS] = { nullptr };
    
    Board* board = nullptr;
    Logic* logic = nullptr;
    Display* display = nullptr;
    Persistence* persistence = nullptr;

    void cleanUp(bool);

    void createCards();
    void deleteCards();
    void shuffleCards();

    void handleArrowKeys(ArrowKey);
    void handleEnterKey();
};