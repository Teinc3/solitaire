#pragma once

#include "common.hpp"

constexpr int MAX_INFO_WIDTH = MIN_WIDTH - 4;

const string TUTORIAL[] = {
    "Tutorial",
    "1. The aim of the game is to move all cards into four suit piles, and they must be arranged in ascending order from Ace to King.",
    "2. The game board consists of piles, namely: the Stock and Waste on the left, the Foundations on the right, and the Tableau at the center.",
    "3. You can flip one card at a time from the Stock onto the Waste. You can move the top card of the Waste onto the Foundations or the Tableau.",
    "4. Cards in the Tableau can be moved onto Foundations or onto other Tableau piles. They are arranged in descending order and must have alternating suit colors.",
    "5. Only Kings can be placed in empty Tableau spots.",
    "6. The game is won after all cards are moved onto the Foundations.",
    "7. Try to use as few moves as possible. Good luck!"
};
const string CONTROLS[] = {
    "Controls",
    "- Arrow Keys to move the cursor or change pagination",
    "- Enter key to lock the cursor or make a move",
    "- Backspace/Delete key to toggle Menu or dismiss errors"
};
const string ABOUT[] = {
    "About",
    "SOLITAIRE by Teinc3",
    "v1.0.0 - May 21,2024",
    "Created for COMP2113/ENGG1340 Freeriders",
};
constexpr int SECTION_LENGTH[3] = { 8, 4, 4 };

class Info
{
public:
    Info();

    void shiftRow(bool);
    void render();

    GameState getPrevMenuState();
    void setPrevMenuState(GameState);

private:
    GameState prevMenuState;

    int topRowIndex;
    int rowCount;
    string infoText;

    void parseInfoText();

    void addSectionText(int);
    void addHorizontalDelimiter();
};