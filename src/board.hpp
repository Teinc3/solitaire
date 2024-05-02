#pragma once

#include "common.hpp"
#include "card.hpp"

class Board
{
public:
    Board();
    ~Board();

    void cleanup();
    void distributeCards(Card* deck[MAX_CARDS]);

    void addCardToStack(int stackIndex, Card* card);
    Card* removeCardFromStack(int stackIndex);
    void addCardToFoundation(int foundationIndex, Card* card);
    Card* removeCardFromFoundation(int foundationIndex);

    int getStackLength(int stackIndex);
    int getFoundationLength(int foundationIndex);

    Card* getCardFromStack(int stackIndex, int cardIndex);
    Card* getCardFromFoundation(int foundationIndex, int cardIndex);
    Card* getNextUnusedCard();
    
private:
    std::vector<Card*>* stacks[STACK_COUNT];
    std::vector<Card*>* foundations[FOUNDATION_COUNT];
    std::vector<Card*> unusedCards; // For the rest of the cards

    int unusedCardIndex; // Tells us which card we are at in the unusedCards vector
};