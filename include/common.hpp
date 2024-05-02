#pragma once

#include <vector>
#include <iostream>
#include <string>

using std::string;

const int MAX_CARDS = 52;
const int SUIT_COUNT = 4;
const int MAX_VALUE = 13;

const int STACK_COUNT = 7;
const int FOUNDATION_COUNT = 4;
const int RESERVED_CARDS = 24;

enum Suit
{
    DIAMONDS,
    CLUBS,
    HEARTS,
    SPADES
};