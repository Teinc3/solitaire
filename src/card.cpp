#include "card.hpp"

Card::Card(Suit suit, int value)
{
    this->suit = suit;
    this->value = value;
    this->isFaceUp = false;
}

Suit Card::getSuit()
{
    return this->suit;
}

int Card::getValue()
{
    return this->value;
}

bool Card::getIsFaceUp()
{
    return this->isFaceUp;
}

bool Card::getIsRed()
{
    return this->suit == Suit::HEARTS || this->suit == Suit::DIAMONDS;
}

void Card::setIsFaceUp(bool isFaceUp)
{
    this->isFaceUp = isFaceUp;
}