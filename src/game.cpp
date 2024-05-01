#include "game.hpp"

Game::Game()
{
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
}

void Game::cleanUp()
{
    // Clean up the game
    deleteCards();
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
    std::cout << "Cards:" << std::endl;
    for (int i = 0; i < 52; ++i) {
        Suit suit = deck[i]->getSuit();
        int value = deck[i]->getValue();
        std::cout << i << ": " << suit << " " << value << std::endl;
    }

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