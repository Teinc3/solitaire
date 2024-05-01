#pragma once

#include "common.hpp"

class Card
{
public:
    Card(Suit suit, int value);

    Suit getSuit();
    int getValue();
    bool getIsFaceUp();
    bool getIsRed();

    void setIsFaceUp(bool isFaceUp);
private:
    Suit suit;
    int value;
    bool isFaceUp;
};