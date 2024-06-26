#include "game.hpp"
#include "board.hpp"
#include "display.hpp"
#include "logic.hpp"

Game::Game()
{   
    // Initialize the game loop
    this->isRunning = true;
    srand(time(0));

    this->gameState = GameState::MAIN_MENU;
    this->menuOption = MenuOption::NEW_GAME;

    this->board = new Board();
    this->display = new Display(this);
    this->logic = new Logic(this->board, this->display);
    this->persistence = new Persistence(this->board, this->deck);

    this->isGamePreviouslyCreated = false;
    this->hasAlreadyWon = false;
}

Game::~Game()
{
    cleanUp(true);
}

void Game::createGame(bool fromLoad)
{   
    // Reset the game
    if (this->isGamePreviouslyCreated)
    {
        cleanUp(false);
    }
    else
    {
        this->isGamePreviouslyCreated = true;
    }

    this->gameState = GameState::PLAYING;
    this->hasAlreadyWon = false;
    this->hasAlreadyPromptedAutoFinished = false;

    this->display->onNewGame();
    this->board->onNewGame();

    createCards();

    if (!fromLoad)
    {
        shuffleCards();
        board->distributeCards(this->deck);
    }
}

void Game::finishGame()
{
    // Clean up every stack
    for (int i = 0; i < STACK_COUNT; i++)
    {
        int stackLength = this->board->getStackLength(i);
        for (int j = 0; j < stackLength; j++)
        {
            Card* card = this->board->removeCardFromStack(i);
            if (card->value == MAX_VALUE)
            {
                this->board->addCardToFoundation(card->suit, card);
            }
        }
    }

    this->hasAlreadyWon = true;
    this->display->setMessage(1);
}

void Game::update()
{
    // Update the game
    if (this->gameState != GameState::PLAYING)
    {
        return;
    }

    this->board->flipTopStackCards();
    this->display->getCursor()->clampCursorPiles();

    // Logic checks
    if (this->logic->isGameWon())
    {
        if (this->hasAlreadyWon)
        {
            return;
        }
        this->display->setMessage(1);
        this->hasAlreadyWon = true;
    }
    else if (this->logic->canAutoFinish())
    {
        if (this->hasAlreadyPromptedAutoFinished == true)
        {
            return;
        }
        this->display->setMessage(3);
        this->hasAlreadyPromptedAutoFinished = true;
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

    if (ch == 8 || ch == 127)
    {   
        // Backspace/del key
        if (this->display->resetMessage(true))
        {
            return;
        }

        if (this->gameState == GameState::INFO_PAGE)
        {
            // Regress to the previous menu
            this->gameState = this->display->getInfo()->getPrevMenuState();
        }
        else if (this->gameState == GameState::PLAYING)
        {
            this->gameState = this->hasAlreadyWon ? GameState::MAIN_MENU : GameState::GAME_MENU;
        }
        return;
    }

    if (WINDOWS && ch >= PSArrowKey::_UP && ch <= PSArrowKey::_DOWN)
    {
        // Map to arrow Keys if powershell
        switch (ch)
        {
        case PSArrowKey::_UP:
            handleArrowKeys(ArrowKey::UP);
            break;
        case PSArrowKey::_DOWN:
            handleArrowKeys(ArrowKey::DOWN);
            break;
        case PSArrowKey::_RIGHT:
            handleArrowKeys(ArrowKey::RIGHT);
            break;
        case PSArrowKey::_LEFT:
            handleArrowKeys(ArrowKey::LEFT);
            break;
        }
    }
    else if (ch >= ArrowKey::DOWN && ch <= ArrowKey::RIGHT)
    {
        handleArrowKeys(static_cast<ArrowKey>(ch));
    }
}

bool Game::getIsRunning()
{
    return this->isRunning;
}

bool Game::getHasAlreadyWon()
{
    return this->hasAlreadyWon;
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

void Game::cleanUp(bool hardCleanUp) // If hardCleanUp is true, delete all objects
{
    // Clean up the game
    deleteCards();

    if (hardCleanUp)
    {
        delete this->display;
        this->display = nullptr;

        delete this->display;
        this->display = nullptr;

        delete this->logic;
        this->logic = nullptr;

        delete this->persistence;
        this->persistence = nullptr;
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
        this->deck[i] = new Card{ static_cast<Suit>(i / 13), i % 13 + 1} ;
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


void Game::handleArrowKeys(ArrowKey arrowKey)
{
    // Handle arrow key presses here
    switch (arrowKey)
    {
    case ArrowKey::UP:
        switch (this->gameState)
        {
        case GameState::PLAYING:
            // Increase the vertical index of the cursor
            this->display->getCursor()->updateVerticalCursorIndex(true);
            break;
        case GameState::INFO_PAGE:
            this->display->getInfo()->shiftRow(false);
            break;
        default:
            if (this->menuOption > MenuOption::NEW_GAME)
            {
                this->menuOption = static_cast<MenuOption>(this->menuOption - 1);
            }
        }
        break;
    case ArrowKey::DOWN:
        switch (this->gameState)
        {
        case GameState::PLAYING:
            // Increase the vertical index of the cursor
            this->display->getCursor()->updateVerticalCursorIndex(false);
            break;
        case GameState::INFO_PAGE:
            this->display->getInfo()->shiftRow(true);
            break;
        default:
            if (this->menuOption < MenuOption::QUIT)
            {
                this->menuOption = static_cast<MenuOption>(this->menuOption + 1);
            }
        }
        break;
    case ArrowKey::RIGHT:
        if (this->gameState == GameState::PLAYING)
        {
            this->display->getCursor()->updateHorizCursorX(true);
        }
        break;
    case ArrowKey::LEFT:
        if (this->gameState == GameState::PLAYING)
        {
            this->display->getCursor()->updateHorizCursorX(false);
        }
        break;
    default:
        break;
    }
}

void Game::handleEnterKey()
{   
    if (this->display->resetMessage(false))
    {
        return;
    }
    if (this->gameState <= GameState::GAME_MENU)
    {
        int result = 0;

        switch (this->menuOption)
        {
        case MenuOption::NEW_GAME:
            if (this->gameState == GameState::MAIN_MENU || this->hasAlreadyWon)
            {
                createGame(false);
            }
            else // Continue
            {
                this->gameState = GameState::PLAYING;
            }
            break;

        case MenuOption::LOAD_SAVE_GAME:
            if (this->gameState == GameState::GAME_MENU)
            {
                // Save game
                result = this->persistence->saveFile() ? 3 : 1;
            }
            else // Load game
            {
                createGame(true);
                result = this->persistence->loadFile() ? 4 : 2;

                if (result == 2) // Load failed
                {
                    this->gameState = GameState::MAIN_MENU;
                }
            }

            if (result >= 1)
            {
                flash();
                this->display->setMessage(LOAD_SAVE_MSG_INDEX + result - 1);
            }
            break;

        case MenuOption::INFO:
            this->display->getInfo()->setPrevMenuState(this->gameState);
            this->gameState = GameState::INFO_PAGE;
            break;

        case MenuOption::QUIT:
            if (this->gameState == GameState::MAIN_MENU) // Quit Game
            {
                this->isRunning = false;
            }
            else // Stop Game
            {
                this->gameState = GameState::MAIN_MENU;
            }
            break;
        }
    }
    else
    {
        // Usually confirming an action.
        int horizCursorXIndex = this->display->getCursor()->getHorizCursorXIndex();
        int verticalCursorIndex = this->display->getCursor()->getVerticalCursorIndex();
        bool result;

        if (verticalCursorIndex == -1) // Empty pile should not have any inputs
        {
            result = false;
        }
        else if (horizCursorXIndex == 0) // Unused pile
        {
            this->logic->handleUnusedCardSelection(verticalCursorIndex);
            result = true; // Unless sth weird happens, such as cursor is on an emptied unused pile, shouldnt have errors
        }
        else if (horizCursorXIndex >= 1 && horizCursorXIndex <= STACK_COUNT)
        {
            // Stack
            result = this->logic->handleStackSelection(horizCursorXIndex - 1, this->display->getCursor()->getLockedCursorPileIndex(), verticalCursorIndex);
        }
        else
        {
            // Foundation
            result = this->logic->handleFoundationSelection(this->display->getCursor()->getLockedCursorPileIndex(), verticalCursorIndex);
        }

        // Flash the screen if there was an error
        if (!result)
        {
            flash();
            this->display->getCursor()->updateCursorLock(true);
            this->display->setMessage(ERROR_MSG_INDEX);
        }
    }
}