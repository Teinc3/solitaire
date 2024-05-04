#pragma once

#include "common.hpp"
#include "card.hpp"

class Board
{
public:
    Board();
    ~Board();

    void cleanup();
    void distributeCards(Card* [MAX_CARDS]);

    void flipTopStackCards();

    void addCardToStack(int, Card*);
    Card* removeCardFromStack(int);
    void addCardToFoundation(int, Card*);
    Card* removeCardFromFoundation(int);
    Card* removeUnusedCard();

    int getStackLength(int);
    int getFoundationLength(int);

    Card* getCardFromStack(int, int);
    Card* getCardFromFoundation(int, int);

    Card* getCurrentUnusedCard();
    Card* getNextUnusedCard();
    Card* shiftNextUnusedCard();
    int getRemainingUnusedCardCount();
    
private:
    int unusedCardIndex;

    std::vector<Card*>* stacks[STACK_COUNT];
    std::vector<Card*>* foundations[FOUNDATION_COUNT];
    std::vector<Card*> unusedCards;
};