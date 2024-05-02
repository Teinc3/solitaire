#include "board.hpp"

Board::Board()
{
    this->unusedCardIndex = -1;

    // Initialize 7 stacks
    for (int i = 0; i < STACK_COUNT; i++)
    {
        this->stacks[i] = new std::vector<Card*>;
    }

    // Initialize 4 foundations and reserve 13 spaces for each
    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        this->foundations[i] = new std::vector<Card*>;
        this->foundations[i]->reserve(MAX_VALUE);
    }

    // Reserve 24 card pointers for the unusedCards vector
    this->unusedCards.reserve(RESERVED_CARDS);
}

Board::~Board()
{
    std::cout << "Deleting Board" << std::endl;
    cleanup();
}

void Board::cleanup()
{
    // Clean up the board
    for (int i = 0; i < STACK_COUNT; i++)
    {
        delete this->stacks[i];
    }

    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        delete this->foundations[i];
    }
}

void Board::distributeCards(Card* deck[MAX_CARDS])
{
    // Distribute cards to the stacks
    int cardIndex = 0;
    for (int i = 0; i < STACK_COUNT; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {   
            Card* cardp = deck[cardIndex];
            this->stacks[i]->push_back(cardp);
            if (j == i)
            {
                cardp->setIsFaceUp(true);
            }
            cardIndex++;
        }
    }

    std::cout << "Distributed cards to the stacks" << std::endl;

    // Distribute the rest of the cards to the unusedCards vector
    for (; cardIndex < MAX_CARDS; cardIndex++)
    {
        this->unusedCards.push_back(deck[cardIndex]);
    }
}

void Board::addCardToStack(int stackIndex, Card* card)
{
    // Add a card to a stack
    this->stacks[stackIndex]->push_back(card);
}

Card* Board::removeCardFromStack(int stackIndex)
{
    // Remove a card from a stack
    Card* card = this->stacks[stackIndex]->back();
    this->stacks[stackIndex]->pop_back();
    return card;
}

void Board::addCardToFoundation(int foundationIndex, Card* card)
{
    // Add a card to a foundation
    this->foundations[foundationIndex]->push_back(card);
}

Card* Board::removeCardFromFoundation(int foundationIndex)
{
    // Remove a card from a foundation
    Card* card = this->foundations[foundationIndex]->back();
    this->foundations[foundationIndex]->pop_back();
    return card;
}

int Board::getStackLength(int stackIndex)
{
    // Get the length of a stack
    return this->stacks[stackIndex]->size();
}

int Board::getFoundationLength(int foundationIndex)
{
    // Get the length of a foundation
    return this->foundations[foundationIndex]->size();
}

Card* Board::getCardFromStack(int stackIndex, int cardIndex)
{
    // Get a card from a stack
    return this->stacks[stackIndex]->at(cardIndex);
}

Card* Board::getCardFromFoundation(int foundationIndex, int cardIndex)
{
    // Get a card from a foundation
    return this->foundations[foundationIndex]->at(cardIndex);
}

Card* Board::getNextUnusedCard()
{
    this->unusedCardIndex++;
    if (this->unusedCardIndex >= this->unusedCards.size())
    {
        this->unusedCardIndex = -1;
        return nullptr;
    }
    return this->unusedCards.at(this->unusedCardIndex);
}