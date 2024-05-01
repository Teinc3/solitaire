#include "card.hpp"

Card::Card(Suit suit, int value)
{
    this->suit = suit;
    this->value = value;
    isFaceUp = false;
}

Suit Card::getSuit()
{
    return suit;
}

int Card::getValue()
{
    return value;
}

bool Card::getIsFaceUp()
{
    return isFaceUp;
}

bool Card::getIsRed()
{
    return suit == Suit::HEARTS || suit == Suit::DIAMONDS;
}

void Card::setIsFaceUp(bool isFaceUp)
{
    this->isFaceUp = isFaceUp;
}