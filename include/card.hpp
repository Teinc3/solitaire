#pragma once

#include "common.hpp"

class Card
{
public:
    Card(Suit, int);

    Suit getSuit();
    int getValue();
    bool getIsFaceUp();
    bool getIsRed();
    string getSuitString();
    
    void setIsFaceUp(bool);
    
private:
    Suit suit;
    int value;
    bool isFaceUp;
};