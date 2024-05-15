#include "board.hpp"

Board::Board()
{
    onNewGame();
}

Board::~Board()
{
    cleanup();
}

void Board::cleanup()
{
    // Clean up the board
    for (int i = 0; i < STACK_COUNT; i++)
    {
        delete this->stacks[i];
        this->stacks[i] = nullptr;
    }

    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        delete this->foundations[i];
        this->foundations[i] = nullptr;
    }

    delete this->unusedCards;
    this->unusedCards = nullptr;
}

void Board::onNewGame()
{
    this->unusedCardIndex = -1;
    this->moves = 0;

    // Initialize 7 stacks
    for (int i = 0; i < STACK_COUNT; i++)
    {
        this->stacks[i] = new vector<Card*>;
    }

    // Initialize 4 foundations and reserve 13 spaces for each
    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        this->foundations[i] = new vector<Card*>;
        this->foundations[i]->reserve(MAX_VALUE);
    }

    // Reserve 24 card pointers for the unusedCards vector
    this->unusedCards = new vector<Card*>;
    this->unusedCards->reserve(RESERVED_CARDS);
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

    // Distribute the rest of the cards to the unusedCards vector
    for (; cardIndex < MAX_CARDS; cardIndex++)
    {
        this->unusedCards->push_back(deck[cardIndex]);
    }
}

void Board::flipTopStackCards()
{
    for (int i = 0; i < STACK_COUNT; i++)
    {
        int stackLength = this->stacks[i]->size();
        if (stackLength > 0)
        {
            Card* card = this->stacks[i]->at(stackLength - 1);
            if (!card->getIsFaceUp())
            {
                card->setIsFaceUp(true);
            }
        }
    }
}

void Board::addCardToStack(int stackIndex, Card* card)
{
    this->stacks[stackIndex]->push_back(card);
    card->setIsFaceUp(true);
}

Card* Board::removeCardFromStack(int stackIndex)
{
    // Remove a card from a stack
    Card* card = this->stacks[stackIndex]->back();
    this->stacks[stackIndex]->pop_back();
    return card;
}

Card* Board::removeUnusedCard()
{
    // Remove an unused card
    if (this->unusedCardIndex == -1)
    {
        return nullptr;
    }
    Card* card = this->unusedCards->at(this->unusedCardIndex);
    this->unusedCards->erase(this->unusedCards->begin() + this->unusedCardIndex);
    unusedCardIndex--;
    
    return card;
}

void Board::addCardToFoundation(int foundationIndex, Card* card)
{
    // Add a card to a foundation
    this->foundations[foundationIndex]->push_back(card);
    card->setIsFaceUp(true);
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

// This shows the current card that the user can "use"
Card* Board::getCurrentUnusedCard()
{
    // Get the current unused card
    if (this->unusedCardIndex == -1)
    {
        return nullptr;
    }
    return this->unusedCards->at(this->unusedCardIndex);
}

Card* Board::getNextUnusedCard()
{
    // Get the next unused card
    if (this->unusedCardIndex + 1 >= static_cast<int>(this->unusedCards->size()))
    {
        return nullptr;
    }
    return this->unusedCards->at(this->unusedCardIndex + 1);
}

Card* Board::shiftNextUnusedCard()
{
    // Hide the last card
    if (this->unusedCardIndex != -1)
    {
        this->unusedCards->at(this->unusedCardIndex)->setIsFaceUp(false);
    }
    
    this->unusedCardIndex++;
    if (this->unusedCardIndex >= static_cast<int>(this->unusedCards->size()))
    {
        this->unusedCardIndex = -1;
        return nullptr;
    }
    
    Card* card = this->unusedCards->at(this->unusedCardIndex);
    card->setIsFaceUp(true);

    return card;
}

int Board::getRemainingUnusedCardCount()
{
    // Get the remaining unused card count
    return this->unusedCards->size() - this->unusedCardIndex - 1;
}

int Board::getMoves()
{
    return this->moves;
}

void Board::addMoves()
{
    this->moves++;
}