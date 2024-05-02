#include "card.hpp"

Card::Card(Suit suit, int value)
{
    this->suit = suit;
    this->value = value;
    isFaceUp = false;
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

string Card::getSuitString()
{
    switch (this->suit)
    {
    case Suit::DIAMONDS:
        return "♦";
    case Suit::CLUBS:
        return "♣";
    case Suit::HEARTS:
        return "♥";
    case Suit::SPADES:
        return "♠";
    default:
        return "N/A";
    }
}

void Card::setIsFaceUp(bool isFaceUp)
{
    this->isFaceUp = isFaceUp;
}