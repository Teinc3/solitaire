#include "logic.hpp"

Logic::Logic(Board* board, Display* display)
{
    this->board = board;
    this->display = display;
}

Logic::~Logic()
{
    this->board = nullptr;
    this->display = nullptr;
}

void Logic::handleUnusedCardSelection(int verticalCursorIndex)
{
    // If Cursor is on ?/X, shift to next card
    if (verticalCursorIndex == 0)
    {
        this->board->shiftNextUnusedCard();
    }
    else // lock cursor
    {
        this->display->updateCursorLock(true);
    }
}

// Move cards from one stack to another stack
// We can use enum to represent the return values and tell the player what went wrong
bool Logic::stackToStack(int cardIndex, int fromStackIndex, int toStackIndex)
{
    // Get the stack length
    int fromStackLength = this->board->getStackLength(toStackIndex);
    if (cardIndex >= fromStackLength)
    {
        return false;
    }

    // Get the cards that will be moved from the stack
    // Example: If the stack has 5 cards and the cardIndex is 2, then 3 cards will be moved (2, 3, 4)
    int numCardsToMove = fromStackLength - cardIndex;
    Card* cardsToMove[numCardsToMove];
    for (int i = 0; i < numCardsToMove; ++i)
    {
        cardsToMove[i] = this->board->getCardFromStack(fromStackIndex, cardIndex + i);
        // If any of the cards are face down, then we can't move them
        if (!cardsToMove[i]->getIsFaceUp())
        {
            return false;
        }
    }

    // If toStack is empty, then we can move only King-stacks
    if (this->board->getStackLength(toStackIndex) == 0)
    {
        if (canEmptyStackAcceptCard(cardsToMove[0]) == false)
        {
            return false;
        }
    }
    else
    {
        // If toStack is not empty, then we can move only cards that are in descending order and alternate colors
        Card* toTopCard = this->board->getCardFromStack(toStackIndex, this->board->getStackLength(toStackIndex) - 1);
        if (canExistingStackAcceptCard(toTopCard, cardsToMove[0]) == false)
        {
            return false;
        }
    }

    // Move the cards to the stack
    for (int i = 0; i < numCardsToMove; i++)
    {
        this->board->removeCardFromStack(fromStackIndex);
        this->board->addCardToStack(toStackIndex, cardsToMove[i]);
    }
    return true;
}

bool Logic::stackToFoundation(int stackIndex)
{
    // Get the stack length
    int stackLength = this->board->getStackLength(stackIndex);
    if (stackLength == 0)
    {
        return false;
    }

    // Get the card that will be moved from the stack
    Card* card = this->board->getCardFromStack(stackIndex, stackLength - 1);
    if (!card->getIsFaceUp())
    {
        return false;
    }

    // Get suit of the card
    int cardSuit = static_cast<int>(card->getSuit());
    int foundationLength = this->board->getFoundationLength(cardSuit);
    if (foundationLength == 0)
    {
        // If the foundation is empty, then we can move only Ace-cards
        if (canEmptyFoundationAcceptCard(card) == false)
        {
            return false;
        }
    }
    else
    {
        // If the foundation is not empty, then we can move only cards that are in ascending order and same suit
        Card* foundationTopCard = this->board->getCardFromFoundation(cardSuit, foundationLength - 1);
        if (canExistingFoundationAcceptCard(foundationTopCard, card) == false)
        {
            return false;
        }
    }

    // Move the card to the foundation
    this->board->removeCardFromStack(stackIndex);
    this->board->addCardToFoundation(cardSuit, card);
    return true;
}

bool Logic::unusedToStack(int stackIndex)
{
    // Get the stack length
    int stackLength = this->board->getStackLength(stackIndex);
    if (stackLength > 0)
    {
        return false;
    }

    // Get the card that will be moved from the unused cards
    Card* card = this->board->getCurrentUnusedCard();
    if (card == nullptr)
    {
        return false;
    }

    // Move the card to the stack
    this->board->removeUnusedCard();
    this->board->addCardToStack(stackIndex, card);
    return true;
}

bool Logic::unusedToFoundation()
{
    // Get the card that will be moved from the unused cards
    Card* card = this->board->getCurrentUnusedCard();
    if (card == nullptr)
    {
        return false;
    }

    // Get suit of the card
    int cardSuit = static_cast<int>(card->getSuit());
    int foundationLength = this->board->getFoundationLength(cardSuit);
    if (foundationLength == 0)
    {
        // If the foundation is empty, then we can move only Ace-cards
        if (canEmptyFoundationAcceptCard(card) == false)
        {
            return false;
        }
    }
    else
    {
        // If the foundation is not empty, then we can move only cards that are in ascending order and same suit
        Card* foundationTopCard = this->board->getCardFromFoundation(cardSuit, foundationLength - 1);
        if (canExistingFoundationAcceptCard(foundationTopCard, card) == false)
        {
            return false;
        }
    }

    // Move the card to the foundation
    this->board->removeUnusedCard();
    this->board->addCardToFoundation(cardSuit, card);
    return true;
}

bool Logic::foundationToStack(int foundationIndex, int stackIndex)
{
    // Get the stack length
    int stackLength = this->board->getStackLength(stackIndex);
    if (stackLength == 0)
    {
        return false;
    }

    // Get the card that will be moved from the foundation
    Card* card = this->board->getCardFromFoundation(foundationIndex, this->board->getFoundationLength(foundationIndex) - 1);
    if (!card->getIsFaceUp())
    {
        return false;
    }

    // If toStack is empty, then we can move only King-stacks
    if (this->board->getStackLength(stackIndex) == 0)
    {
        if (canEmptyStackAcceptCard(card) == false)
        {
            return false;
        }
    }
    else
    {
        // If toStack is not empty, then we can move only cards that are in descending order and alternate colors
        Card* toTopCard = this->board->getCardFromStack(stackIndex, stackLength - 1);
        if (canExistingStackAcceptCard(toTopCard, card) == false)
        {
            return false;
        }
    }

    // Move the card to the stack
    this->board->removeCardFromFoundation(foundationIndex);
    this->board->addCardToStack(stackIndex, card);
    return true;
}

bool Logic::canExistingStackAcceptCard(Card* toCard, Card* fromCard)
{
    return (toCard->getValue() - 1 == fromCard->getValue()) && (toCard->getIsRed() != fromCard->getIsRed());
}

bool Logic::canEmptyStackAcceptCard(Card* card)
{
    return card->getValue() == 13;
}

bool Logic::canEmptyFoundationAcceptCard(Card* card)
{
    return card->getValue() == 1;
}

bool Logic::canExistingFoundationAcceptCard(Card* toCard, Card* fromCard)
{
    return (toCard->getValue() + 1 == fromCard->getValue()) && (toCard->getSuit() == fromCard->getSuit());
}