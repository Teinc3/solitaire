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

// First check isGameWon before checking canAutoFinish
bool Logic::isGameWon()
{
    // Check if all the foundation piles are full
    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        if (this->board->getFoundationLength(i) != MAX_VALUE)
        {
            return false;
        }
    }

    // If all the foundation piles are full, then the game is won
    return true;
}

bool Logic::canAutoFinish()
{
    // No more unused cards, and every card in the stack is face up
    if (this->board->getCurrentUnusedCard() != nullptr || this->board->getNextUnusedCard() != nullptr)
    {
        return false;
    }
    
    for (int i = 0; i < STACK_COUNT; i++)
    {
        int stackLength = this->board->getStackLength(i);
        if (stackLength == 0)
        {
            continue;
        }

        Card* bottomCard = this->board->getCardFromStack(i, 0);
        if (!bottomCard->getIsFaceUp())
        {
            return false;
        }
    }

    return true;
}

void Logic::handleUnusedCardSelection(int verticalCursorIndex)
{
    // If Cursor is on ?/X, shift to next card
    if (verticalCursorIndex == 0)
    {
        this->board->shiftNextUnusedCard();
        this->board->addMoves();
    }
    else // lock or unlock cursor
    {
        this->display->getCursor()->updateCursorLock(true);
    }
}

// This action means that the player wants to move cards from some place to this stack
bool Logic::handleStackSelection(int toStackIndex, int fromPileIndex, int verticalCursorIndex)
{
    // First we determine the index of the card
    // Vertical Cursor Index determines the card index in the stack
    if (fromPileIndex == 0) // Unused Pile
    {
        // We know its current unused card, as you can't lock cursor on a hidden/X tile
        return unusedToStack(toStackIndex);
    }
    else if (fromPileIndex <= STACK_COUNT) // Stack
    {
        int fromStackIndex = fromPileIndex - 1;
        if (fromStackIndex == toStackIndex) // Lock vertical cursor or Unlock
        {
            this->display->getCursor()->updateCursorLock(true);
            return true;
        }

        // We might have to loop through the stack again ffs...
        int stackLength = this->board->getStackLength(fromStackIndex);
        if (verticalCursorIndex >= stackLength)
        {
            return false;
        }

        int hiddenCount = 0;
        for (int i = 0; i < stackLength; i++)
        {
            Card* card = this->board->getCardFromStack(fromStackIndex, i);
            if (!card->getIsFaceUp())
            {
                hiddenCount++;
            }
        }
        // If there is a hidden card, means 0 to hiddenCount - 1 are hidden and collapsed to verticalCursorIndex 0
        hiddenCount = hiddenCount > 0 ? hiddenCount - 1 : 0;
        int cardIndex = verticalCursorIndex + hiddenCount;

        return stackToStack(cardIndex, fromStackIndex, toStackIndex);
    }
    else // We transferring from foundation
    {
        int foundationPileIndex = fromPileIndex - 1 - STACK_COUNT;
        int foundationIndex = foundationPileIndex + 2 * verticalCursorIndex;
        
        return foundationToStack(foundationIndex, toStackIndex);
    }
}

bool Logic::handleFoundationSelection(int cursorPileIndex, int verticalCursorIndex)
{
    /* Possibilities
    1. Cursor is on foundation pile - We swap cursor status
    2. Cursor is on stack (Locked) - We try to move the cards from the stack to the foundation pile
    3. Cursor is on unused pile - We try to move the card from the unused pile to the foundation pile
    */
   
    if (cursorPileIndex == 0) // Unused Pile
    {
        return unusedToFoundation();
    }
    else if (cursorPileIndex <= STACK_COUNT) // Stack
    {
        int stackIndex = cursorPileIndex - 1;
        return stackToFoundation(stackIndex);
    }
    else // Foundation
    {
        this->display->getCursor()->updateCursorLock(true);
        return true;
    }
}

// Move cards from one stack to another stack
// We can use enum to represent the return values and tell the player what went wrong
bool Logic::stackToStack(int cardIndex, int fromStackIndex, int toStackIndex)
{
    // Get the stack length
    int fromStackLength = this->board->getStackLength(fromStackIndex);
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
    // Unlock the cursor
    this->display->getCursor()->updateCursorLock(true);
    this->board->addMoves();
    return true;
}

bool Logic::stackToFoundation(int stackIndex)
{
    bool hasTransferredCard = false;
    
    while (true)
    {
        // Get the stack length
        int stackLength = this->board->getStackLength(stackIndex);
        if (stackLength == 0)
        {
            break;
        }

        // Get the card that will be moved from the stack
        Card* card = this->board->getCardFromStack(stackIndex, stackLength - 1);
        if (!card->getIsFaceUp())
        {
            break;
        }

        // Get suit of the card
        int cardSuit = static_cast<int>(card->getSuit());
        int foundationLength = this->board->getFoundationLength(cardSuit);
        if (foundationLength == 0)
        {
            // If the foundation is empty, then we can move only Ace-cards
            if (canEmptyFoundationAcceptCard(card) == false)
            {
                break;
            }
        }
        else
        {
            // If the foundation is not empty, then we can move only cards that are in ascending order and same suit
            Card* foundationTopCard = this->board->getCardFromFoundation(cardSuit, foundationLength - 1);
            if (canExistingFoundationAcceptCard(foundationTopCard, card) == false)
            {
                break;
            }
        }

        // Move the card to the foundation
        this->board->removeCardFromStack(stackIndex);
        this->board->addCardToFoundation(cardSuit, card);
        hasTransferredCard = true;
    }

    if (hasTransferredCard)
    {
        // Unlock the cursor
        this->display->getCursor()->updateCursorLock(true);
        this->board->addMoves();
        return true;
    }
    else
    {
        return false;
    }
}

bool Logic::unusedToStack(int stackIndex)
{
    // Get the card that will be moved from the unused cards
    Card* card = this->board->getCurrentUnusedCard();
    if (card == nullptr)
    {
        return false;
    }

    // Get the stack length
    int stackLength = this->board->getStackLength(stackIndex);

    // Check the last card in the stack and see if it is compatible
    if (stackLength > 0)
    {
        Card* toTopCard = this->board->getCardFromStack(stackIndex, stackLength - 1);
        if (toTopCard == nullptr || !canExistingStackAcceptCard(toTopCard, card))
        {
            return false;
        }
    }

    // Move the card to the stack
    this->board->removeUnusedCard();
    this->board->addCardToStack(stackIndex, card);

    // Unlock the cursor
    this->display->getCursor()->updateCursorLock(true);
    this->board->addMoves();
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

    // Unlock the cursor
    this->display->getCursor()->updateCursorLock(true);
    this->board->addMoves();
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

    // Unlock the cursor
    this->display->getCursor()->updateCursorLock(true);
    this->board->addMoves();
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