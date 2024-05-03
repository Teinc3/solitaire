#include "game.hpp"
#include "display.hpp"

Game::Game()
{   
    this->board = new Board();
    this->logic = new Logic(this->board);
    this->display = new Display(this);

    // Initialize the game loop
    this->isRunning = true;
    srand(time(0));

    this->gameState = GameState::MAIN_MENU;
    this->menuOption = MenuOption::NEW_GAME;
}

Game::~Game()
{
    cleanUp();
}

void Game::createGame()
{   
    // Reset the game
    if (this->gameState >= GameState::PLAYING)
    {
        cleanUp();
    }

    this->gameState = GameState::PLAYING;

    createCards();
    shuffleCards();

    board->distributeCards(this->deck);
}

void Game::cleanUp()
{
    // Clean up the game
    deleteCards();

    delete this->board;
    delete this->logic;
    delete this->display;

    this->board = nullptr;
    this->logic = nullptr;
    this->display = nullptr;
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
    //this->board->flipTopStackCards();

    /* std::cout << "Card Distribution:" << std::endl;
    // Get cards from all stacks
    for (int i = 0; i < STACK_COUNT; i++)
    {   
        int stackLength = this->board->getStackLength(i);
        std::cout << "Stack " << i + 1 << "[" << stackLength << "] : ";
        for (int j = 0; j < stackLength; j++)
        {
            Card* card = this->board->getCardFromStack(i, j);
            std::cout << card->getSuit() << "-" << card->getValue() << " ";
        }
        std::cout << std::endl;
    }

    // Get all Unused cards
    std::cout << "Unused Cards: " << std::endl;
    Card* card = this->board->getNextUnusedCard();
    while (card != nullptr)
    {
        std::cout << card->getSuit() << "-" << card->getValue() << " ";
        card = this->board->getNextUnusedCard();
    }
    std::cout << std::endl; */
}

void Game::handleInput()
{
    nodelay(stdscr, TRUE);  // Make getch non-blocking (don't wait for input)
    int ch = getch();  // Get the input from the user

    if (ch == ERR)
    {
        return;
    }

    if (ch == 27) // If the first character is ESC
    {
        getch();  // Ignore the [
        switch (getch()) {  // The third character determines the arrow key
        case 'A':  // Up arrow
            if (this->menuOption > MenuOption::NEW_GAME)
            {
                this->menuOption = static_cast<MenuOption>(this->menuOption - 1);
            }
            break;
        case 'B':  // Down arrow
            if (this->menuOption < MenuOption::QUIT)
            {
                this->menuOption = static_cast<MenuOption>(this->menuOption + 1);
            }
            break;
        default:
            break;
        }
    }
    else if (ch == '\n') // Enter key
    {
        // Handle Enter key press here
        switch (this->menuOption)
        {
        case MenuOption::NEW_GAME:
            createGame();
            break;
        case MenuOption::QUIT:
            this->isRunning = false;
            break;
        default:
            break;
        }
    }
}

bool Game::getIsRunning()
{
    return this->isRunning;
}

Display* Game::getDisplay()
{
    return this->display;
}

GameState Game::getGameState()
{
    return this->gameState;
}

MenuOption Game::getMenuOption()
{
    return this->menuOption;
}