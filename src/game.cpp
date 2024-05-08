#include "game.hpp"
#include "display.hpp"
#include "board.hpp"

Game::Game()
{   
    // Initialize the game loop
    this->isRunning = true;
    srand(time(0));

    this->gameState = GameState::MAIN_MENU;
    this->menuOption = MenuOption::NEW_GAME;

    this->board = new Board();
    this->logic = new Logic(this->board);
    this->display = new Display(this);

    this->isGamePreviouslyCreated = false;
}

Game::~Game()
{
    cleanUp(true);
}

void Game::createGame()
{   
    // Reset the game
    if (this->isGamePreviouslyCreated)
    {
        cleanUp(false);
    }

    this->gameState = GameState::PLAYING;

    createCards();
    shuffleCards();

    board->distributeCards(this->deck);
}

void Game::cleanUp(bool hardCleanUp) // If hardCleanUp is true, delete all objects
{
    // Clean up the game
    deleteCards();

    if (hardCleanUp)
    {
        delete this->board;
        delete this->logic;
        delete this->display;

        this->board = nullptr;
        this->logic = nullptr;
        this->display = nullptr;
    }
    else
    {
        this->board->cleanup();
    }
}

void Game::createCards()
{
    // Create 52 cards
    for (int i = 0; i < 52; ++i) {
        this->deck[i] = new Card(static_cast<Suit>(i / 13), i % 13 + 1);
    }
}

void Game::deleteCards()
{
    // Delete all card objects
    for (int i = 0; i < 52; ++i) {
        if (this->deck[i] != nullptr)
        {
            delete this->deck[i];
            this->deck[i] = nullptr;
        }
    }
    //std::cout << "Deleted all card objects" << std::endl;
}

void Game::shuffleCards()
{
    // Shuffle the deck using the Fisher-Yates algorithm
    for (int i = 51; i > 0; --i) {
        int j = rand() % (i + 1);
        Card* temp = this->deck[i];
        this->deck[i] = this->deck[j];
        this->deck[j] = temp;
    }
}

void Game::update()
{
    // Update the game
    switch (this->gameState)
    {
    case GameState::MAIN_MENU:
        // Handle main menu logic here
        break;
    case GameState::PLAYING:
        this->board->flipTopStackCards();
        this->display->clampCursorPiles();
        break;
    case GameState::GAME_MENU:
        // Handle game menu logic here
        break;
    }
}

void Game::handleInput()
{
    //nodelay(stdscr, TRUE);  // Make getch non-blocking (don't wait for input)
    int ch = getch();  // Get the input from the user
    if (ch == ERR)
    {
        return;
    }

    if (ch == '\n') // Enter key
    {
        // Handle Enter key press here
        handleEnterKey();
        return;
    }

    if ((NCURSES == 1 && ch >= ArrowKey::DOWN && ch <= ArrowKey::RIGHT) || (NCURSES != 1 && ch >= ArrowKey::UP && ch <= ArrowKey::DOWN))
    {
        handleArrowKeys(static_cast<ArrowKey>(ch));
    }
}

bool Game::getIsRunning()
{
    return this->isRunning;
}

GameState Game::getGameState()
{
    return this->gameState;
}

MenuOption Game::getMenuOption()
{
    return this->menuOption;
}

Display* Game::getDisplay()
{
    return this->display;
}

Board* Game::getBoard()
{
    return this->board;
}

void Game::setIsRunning(bool isRunning)
{
    this->isRunning = isRunning;
}

void Game::handleArrowKeys(ArrowKey arrowKey)
{
    // Handle arrow key presses here
    switch (arrowKey)
    {
    case ArrowKey::UP:
        if (this->gameState != GameState::PLAYING && this->menuOption > MenuOption::NEW_GAME)
        {
            this->menuOption = static_cast<MenuOption>(this->menuOption - 1);
        }
        else
        {
            // Decrease the vertical index of the cursor
            this->display->updateVerticalCursorIndex(true);
        }
        break;
    case ArrowKey::DOWN:
        if (this->gameState != GameState::PLAYING && this->menuOption < MenuOption::QUIT)
        {
            this->menuOption = static_cast<MenuOption>(this->menuOption + 1);
        }
        else
        {
            this->display->updateVerticalCursorIndex(false);
        }
        break;
    case ArrowKey::RIGHT:
        if (this->gameState == GameState::PLAYING)
        {
            this->display->updateHorizCursorX(true);
        }
        break;
    case ArrowKey::LEFT:
        if (this->gameState == GameState::PLAYING)
        {
            this->display->updateHorizCursorX(false);
        }
        break;
    default:
        break;
    }
}

void Game::handleEnterKey()
{
    if (this->gameState != GameState::PLAYING)
    {
        switch (this->menuOption)
        {
        case MenuOption::NEW_GAME:
            createGame();
            clear();
            break;
        case MenuOption::QUIT:
            this->isRunning = false;
            break;
        default:
            break;
        }
    }
    else
    {
        // Usually confirming an action.
    }
}