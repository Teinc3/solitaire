#include "cursor.hpp"

Cursor::Cursor(Board* boardPtr)
{
    this->board = boardPtr;

    onNewGame();
}

void Cursor::onNewGame()
{
    this->horizCursorXIndex = 0;
    this->lockedCursorPileIndex = this->horizCursorXIndex;
    this->isLockedCursor = false;

    // Reset pilecursors
    for (int i = 0; i < COL_COUNT; i++)
    {
        this->pileCursors[i] = CursorPileInfo();
    }
}

void Cursor::render()
{
    // Draw the horizontal cursors first
    int baseX = HORIZ_CURSOR_XPOS[this->horizCursorXIndex];
    CursorPileInfo& cursorPile = this->pileCursors[this->horizCursorXIndex];

    if (this->horizCursorXIndex <= STACK_COUNT)
    {
        monoColorPrint(ColorPair::YELLOW, cursorPile.startingY - 1, baseX + 2, "vvv");
        monoColorPrint(ColorPair::YELLOW, cursorPile.startingY + cursorPile.yHeight, baseX + 2, "^^^");
    }
    else
    {
        int yPos = cursorPile.startingY + 1 + cursorPile.currentCursorVerticalIndex * (cursorPile.yHeight + 1);
        monoColorPrint(ColorPair::YELLOW, yPos - 2, baseX + 2, "vvv");
        monoColorPrint(ColorPair::YELLOW, yPos + 2, baseX + 2, "^^^");
    }

    // Now draw vertical cursors
    // Use the locked cursor if it is locked, hence reassign the baseX and cursorPile
    int pileIndex = this->isLockedCursor ? this->lockedCursorPileIndex : this->horizCursorXIndex;
    int yPos;
    baseX = HORIZ_CURSOR_XPOS[pileIndex];
    cursorPile = this->pileCursors[pileIndex];

    if (cursorPile.currentCursorVerticalIndex == -1 && pileIndex <= STACK_COUNT)
    {
        // Pile is empty, don't draw cursor
        return;
    }
    else if (pileIndex == 0) // Unused Pile
    {   
        yPos = cursorPile.startingY + 1 + 2 * cursorPile.currentCursorVerticalIndex;
    }
    else if (pileIndex <= STACK_COUNT) // Stacks
    {
        if (cursorPile.hasHiddenCard)
        {
            // 0 will be the hidden card, 1 will be the first visible card
            if (cursorPile.currentCursorVerticalIndex == 0)
            {
                yPos = cursorPile.startingY + 1;
            }
            else
            {
                yPos = cursorPile.startingY + 2 + cursorPile.currentCursorVerticalIndex;
            }
        }
        else
        {
            yPos = cursorPile.startingY + 1 + cursorPile.currentCursorVerticalIndex;
        }
    }
    else // Foundations
    {
        yPos = cursorPile.startingY + 1 + (cursorPile.currentCursorVerticalIndex) * (cursorPile.yHeight + 1);
    }
    ColorPair color = this->isLockedCursor ? ColorPair::YELLOW : ColorPair::BLUE;
    monoColorPrint(color, yPos, baseX, ">");
    monoColorPrint(color, yPos, baseX + 6, "<");
}


void Cursor::clampCursorPiles()
{   
    for (int i = 0; i < COL_COUNT; i++)
    {
        bool isFoundation = i >= 1 + STACK_COUNT;
        int minVal = -1;
        int maxVal = 0;
        int yHeight = 3;

        this->pileCursors[i].startingY = 2;

        bool hasHiddenCard = false;
        if (i == 0) // Unused pile
        {
            Card* nextCard = this->board->getNextUnusedCard();
            hasHiddenCard = nextCard == nullptr; // | X | means no hidden card

            Card* currCard = this->board->getCurrentUnusedCard();
            if (currCard != nullptr) // 2 rows (X/? + Curr Card)
            {
                minVal = 0;
                maxVal = 1;
                yHeight = 5;
            }
            else if (nextCard != nullptr) // 1 row (?)
            {
                minVal = 0;
                maxVal = 0;
                yHeight = 3;
            }
            else // 0 rows, empty pile
            {
                minVal = -1;
                maxVal = -1;
                yHeight = EMPTY_PILE_CURSORPIILE_YHEIGHT;
            }
        }
        else if (!isFoundation) // Stacks
        {
            int stackIndex = i - 1;
            int stackLength = this->board->getStackLength(stackIndex);
            int visibleCount = 0;

            for (int j = 0; j < stackLength; j++)
            {
                if (this->board->getCardFromStack(stackIndex, j)->isFaceUp)
                {
                    visibleCount++;
                }
                else
                {
                    hasHiddenCard = true;
                }
            }
            maxVal = (hasHiddenCard ? 1 : 0) + visibleCount - 1;
            minVal = maxVal == -1 ? -1 : hasHiddenCard ? 1 : 0;
            yHeight = maxVal == -1 ? EMPTY_PILE_CURSORPIILE_YHEIGHT : ((hasHiddenCard ? 2 : 0) + visibleCount + 2);
        }
        else
        {
            hasHiddenCard = false;
            minVal = 0;
            maxVal = 1;
            yHeight = 3;
        }
        this->pileCursors[i].hasHiddenCard = hasHiddenCard;
        // Here we try not to modify it unless it is over the max value, then we shift it back until it is within bounds
        if (this->pileCursors[i].currentCursorVerticalIndex > maxVal)
        {
            this->pileCursors[i].currentCursorVerticalIndex = maxVal;
        }
        else if (this->pileCursors[i].currentCursorVerticalIndex < minVal)
        {
            this->pileCursors[i].currentCursorVerticalIndex = minVal;
        }
        this->pileCursors[i].yHeight = yHeight;
    }
}

void Cursor::updateHorizCursorX(bool isRight)
{
    if (isRight)
    {
        this->horizCursorXIndex = this->horizCursorXIndex == COL_COUNT - 1 ? 0 : this->horizCursorXIndex + 1;
    }
    else
    {
        this->horizCursorXIndex = this->horizCursorXIndex == 0 ? COL_COUNT - 1 : this->horizCursorXIndex - 1;
    }
    if (!this->isLockedCursor)
    {
        updateCursorLock(false);
    }
}

void Cursor::updateVerticalCursorIndex(bool isUp)
{
    if (!this->isLockedCursor) // Cursor free
    {
        this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex += (isUp ? -1 : 1);
    }
    else if (this->horizCursorXIndex > STACK_COUNT) // Foundation when cursor is not free
    {
        this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex += (isUp ? -1 : 1);
    }
}

void Cursor::updateCursorLock(bool changeCursorStatus)
{
    if (changeCursorStatus)
    {
        this->isLockedCursor = !this->isLockedCursor;
    }
    if (!this->isLockedCursor)
    {
        this->lockedCursorPileIndex = this->horizCursorXIndex;
    }
}

int Cursor::getHorizCursorXIndex()
{
    return this->horizCursorXIndex;
}

int Cursor::getVerticalCursorIndex()
{
    return this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex;
}

int Cursor::getLockedCursorPileIndex()
{
    return this->lockedCursorPileIndex;
}