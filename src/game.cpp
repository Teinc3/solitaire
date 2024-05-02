#include "game.hpp"

Game::Game()
{   
    this->isRunning = false;
    this->board = new Board();

    // Initialize the game
    gameInit();
}

Game::~Game()
{
    cleanUp();
}

void Game::gameInit()
{   
    // Reset the game
    if (this->isRunning)
    {
        cleanUp();
    }

    // Initialize the game
    this->isRunning = true;
    srand(time(0));

    createCards();
    shuffleCards();

    board->distributeCards(this->deck);
}

void Game::cleanUp()
{
    // Clean up the game
    deleteCards();
    delete this->board;
    this->board = nullptr;
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
    std::cout << "Deleted all card objects" << std::endl;
}

void Game::shuffleCards()
{
    // Shuffle the deck using the Fisher-Yates algorithm
    for (int i = 51; i > 0; --i) {
        int j = rand() % (i + 1);
        Card* temp = this->deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void Game::update()
{
    // Update the game
    std::cout << "Card Distribution:" << std::endl;
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
    std::cout << std::endl;

    // End the game
    isRunning = false;
}

Card* (*Game::getDeck())[52]
{
    return &deck;
}

bool Game::getIsRunning()
{
    return isRunning;
}