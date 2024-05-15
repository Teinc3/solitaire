#include "cursor.hpp"

Cursor::Cursor(Board* boardPtr, bool* use2ColFoundationPtr)
{
    this->board = boardPtr;
    this->use2ColFoundationRef = use2ColFoundationPtr;

    onNewGame();
}

void Cursor::onNewGame()
{
    this->horizCursorXIndex = 0;
    this->lockedCursorPileIndex = this->horizCursorXIndex;
    this->isLockedCursor = false;

    // Reset pilecursors
    for (int i = 0; i < COL_COUNT_2COL; i++)
    {
        this->pileCursors[i] = CursorPileInfo();
    }
}

void Cursor::drawCursor()
{
    // Draw the horizontal cursors first
    int baseX = HORIZ_CURSOR_XPOS[this->horizCursorXIndex];
    CursorPileInfo& cursorPile = this->pileCursors[this->horizCursorXIndex];

    if (this->horizCursorXIndex <= STACK_COUNT)
    {
        coloredPrint(YELLOW, cursorPile.startingY - 1, baseX + 2, "vvv");
        coloredPrint(YELLOW, cursorPile.startingY + cursorPile.yHeight, baseX + 2, "^^^");
    }
    else
    {
        int yPos = cursorPile.startingY + 1 + cursorPile.currentCursorVerticalIndex * (cursorPile.yHeight + 1);
        coloredPrint(YELLOW, yPos - 2, baseX + 2, "vvv");
        coloredPrint(YELLOW, yPos + 2, baseX + 2, "^^^");
    }

    // Now draw vertical cursors
    // Use the locked cursor if it is locked, hence reassign the baseX and cursorPile
    int pileIndex = this->isLockedCursor ? this->lockedCursorPileIndex : this->horizCursorXIndex;
    int yPos;
    baseX = HORIZ_CURSOR_XPOS[pileIndex];
    cursorPile = this->pileCursors[pileIndex];

    if (pileIndex == 0) // Unused Pile
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
        yPos = cursorPile.startingY + 1 + cursorPile.currentCursorVerticalIndex * (cursorPile.yHeight + 1);
    }
    // We will want to use pileCursors to check the height of the stack and print the cursor at the appropriate position
    coloredPrint(this->isLockedCursor ? YELLOW : BLUE, yPos, baseX, ">");
    coloredPrint(this->isLockedCursor ? YELLOW : BLUE, yPos, baseX + 6, "<");
}


void Cursor::clampCursorPiles()
{   
    for (int i = 0; i < (this->use2ColFoundationRef ? COL_COUNT_2COL : COL_COUNT_1COL); i++)
    {
        bool isFoundation = i >= 1 + STACK_COUNT;
        int maxVal = 0;
        int yHeight = 3;

        this->pileCursors[i].startingY = 2;

        bool hasHiddenCard = false;
        if (i == 0) // Unused pile
        {
            Card* nextCard = this->board->getNextUnusedCard();
            hasHiddenCard = nextCard == nullptr; // | X | means no hidden card

            Card* currCard = this->board->getCurrentUnusedCard();
            if (currCard != nullptr)
            {
                maxVal = 1;
                yHeight = 5;
            }
        }
        else if (!isFoundation) // Stacks
        {
            int stackIndex = i - 1;
            int stackLength = this->board->getStackLength(stackIndex);
            int visibleCount = 0;

            for (int j = 0; j < stackLength; j++)
            {
                if (this->board->getCardFromStack(stackIndex, j)->getIsFaceUp())
                {
                    visibleCount++;
                }
                else
                {
                    hasHiddenCard = true;
                }
            }

            maxVal = (hasHiddenCard ? 1 : 0) + visibleCount - 1;
            if (maxVal < 0)
            {
                maxVal = 0;
            }
            yHeight = (hasHiddenCard ? 2 : 0) + visibleCount + 2;
        }
        else
        {
            hasHiddenCard = false;
            maxVal = this->use2ColFoundationRef ? 1 : 3;
            yHeight = 3;
        }
        this->pileCursors[i].hasHiddenCard = hasHiddenCard;
        // Here we try not to modify it unless it is over the max value, then we shift it back until it is within bounds
        if (this->pileCursors[i].currentCursorVerticalIndex > maxVal)
        {
            this->pileCursors[i].currentCursorVerticalIndex = maxVal;
        }
        else if (this->pileCursors[i].currentCursorVerticalIndex < 0)
        {
            this->pileCursors[i].currentCursorVerticalIndex = 0;
        }
        this->pileCursors[i].yHeight = yHeight;
    }
}

void Cursor::updateHorizCursorX(bool isRight)
{
    int max_xIndex = this->use2ColFoundationRef ? 9 : 8;
    if (isRight)
    {
        this->horizCursorXIndex = this->horizCursorXIndex == max_xIndex ? 0 : this->horizCursorXIndex + 1;
    }
    else
    {
        this->horizCursorXIndex = this->horizCursorXIndex == 0 ? max_xIndex : this->horizCursorXIndex - 1;
    }
    if (!this->isLockedCursor)
    {
        updateCursorLock(false);
    }
}


void Cursor::updateVerticalCursorIndex(bool isUp)
{
    if (!this->isLockedCursor)
    {
        this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex += isUp ? -1 : 1;
    }
    else if (this->horizCursorXIndex > STACK_COUNT)
    {
        // If we are on a foundation we need to let the user move the cursor up and down
        this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex += isUp ? -1 : 1;
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