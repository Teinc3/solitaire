#pragma once

#include "common.hpp"
#include "card.hpp"

class Board
{
public:
    Board();
    
private:
    std::vector<std::vector<Card>> stacks;
    std::vector<Card> cards; // For the rest of the cards
};