#include "display.hpp"

Display::Display(Game* game)
{   
    initscr();
    keypad(stdscr, TRUE);  // Enable special keys
    cbreak();
    noecho();
    curs_set(0);

    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);  // Get the size of the window

    if (max_x < MIN_WIDTH || max_y < HEIGHT)
    {
        // quit ncurses and print an error message
        endwin();
        game->setIsRunning(false);
        std::cout << "Please increase the size of your terminal to least 71x21 in order to enjoy the game." << std::endl;
        return;
    }
    else if (max_x < MIN_2COL_FOUNDATION_WIDTH)
    {
        this->use2ColFoundation = false;
    }
    else
    {
        this->use2ColFoundation = true;
    }
    this->width = this->use2ColFoundation ? MIN_2COL_FOUNDATION_WIDTH : MIN_WIDTH;

    this->game = game;
    this->horizCursorXIndex = 0;
    this->useUnicode = false;
}

Display::~Display()
{
    endwin();  // End curses mode
}

void Display::render()
{
    clear();

    drawBoundary();

    switch (this->game->getGameState())
    {
    case GameState::MAIN_MENU:
        drawMenu(false);
        break;
    case GameState::PLAYING:
        drawGameBoard();
        break;
    case GameState::GAME_MENU:
        drawMenu(true);
        break;
    default:
        break;
    }

    refresh();
}

void Display::updateHorizCursorX(bool isRight)
{
    int max_xIndex = this->use2ColFoundation ? 9 : 8;
    if (isRight)
    {
        this->horizCursorXIndex = this->horizCursorXIndex == max_xIndex ? 0 : this->horizCursorXIndex + 1;
    }
    else
    {
        this->horizCursorXIndex = this->horizCursorXIndex == 0 ? max_xIndex : this->horizCursorXIndex - 1;
    }
}

void Display::clampCursorPiles()
{   
    for (int i = 0; i < 1 + STACK_COUNT + (use2ColFoundation ? 2 : 1); i++)
    {
        bool isFoundation = i >= 1 + STACK_COUNT;
        int maxVal = 0;
        int yHeight = 3;

        this->pileCursors[i].startingY = 2;

        bool hasHiddenCard = false;
        if (i == 0) // Unused pile
        {
            Card* nextCard = this->game->getBoard()->getNextUnusedCard();
            hasHiddenCard = nextCard == nullptr; // | X | means no hidden card

            Card* currCard = this->game->getBoard()->getCurrentUnusedCard();
            if (currCard != nullptr)
            {
                maxVal = 1;
                yHeight = 5;
            }
        }
        else if (!isFoundation) // Stacks
        {
            int stackIndex = i - 1;
            int stackLength = this->game->getBoard()->getStackLength(stackIndex);
            int visibleCount = 0;

            for (int j = 0; j < stackLength; j++)
            {
                if (this->game->getBoard()->getCardFromStack(stackIndex, j)->getIsFaceUp())
                {
                    visibleCount++;
                }
                else
                {
                    hasHiddenCard = true;
                }
            }

            maxVal = (hasHiddenCard ? 1 : 0) + visibleCount - 1;
            yHeight = (hasHiddenCard ? 2 : 0) + visibleCount + 2;
        }
        else
        {
            hasHiddenCard = false;
            maxVal = this->use2ColFoundation ? 1 : 3;
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

void Display::updateVerticalCursorIndex(bool isUp)
{
    this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex += isUp ? -1 : 1;
}

void Display::drawBoundary()
{

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (i == 0 || i == HEIGHT - 1)
            {
                mvprintw(i, j, "#");  // Print # on the top and bottom
            }
            else if (j == 0 || j == this->width - 1)
            {
                mvprintw(i, j, "*");  // Print * on the sides
            }
        }
    }
}

void Display::drawMenu(bool isGameMenu) {
    int start_y = (HEIGHT - 9) / 2;  // Calculate the starting y position
    int start_x = (this->width - 27) / 2;  // Calculate the starting x position

    MenuOption menuOption = this->game->getMenuOption();
    mvprintw(start_y + 0, start_x, "+=========================+");
    mvprintw(start_y + 1, start_x, "|        SOLITAIRE        |");
    mvprintw(start_y + 2, start_x, "|        by Teinc3        |");
    mvprintw(start_y + 3, start_x, "+-------------------------+");
    mvprintw(start_y + 4, start_x, "|    %s1. New Game%s      |", menuOption == MenuOption::NEW_GAME ? "> " : "  ", menuOption == MenuOption::NEW_GAME ? " <" : "  ");
    mvprintw(start_y + 5, start_x, "|    %s2. %s%s Game%s     |", menuOption == MenuOption::LOAD_SAVE_GAME ? "> " : "  ", isGameMenu ? "Save" : "Load", "", menuOption == MenuOption::LOAD_SAVE_GAME ? " <" : "  ");
    mvprintw(start_y + 6, start_x, "|   %s3. Information%s    |", menuOption == MenuOption::INFO ? "> " : "  ", menuOption == MenuOption::INFO ? " <" : "  ");
    mvprintw(start_y + 7, start_x, "|      %s4. Quit%s        |", menuOption == MenuOption::QUIT ? "> " : "  ", menuOption == MenuOption::QUIT ? " <" : "  ");
    mvprintw(start_y + 8, start_x, "+=========================+");

    refresh();  // Refresh the screen to show the menu
}

// Reference docs/game_design.txt for design
void Display::drawGameBoard()
{
    drawDelimiter(9);
    drawDelimiter(61);

    drawUnusedPile();
    for (int i = 0; i < STACK_COUNT; i++)
    {
        drawStack(i);
    }
    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        drawFoundation(static_cast<Suit>(i));
    }

    drawCursor();
}

void Display::drawDelimiter(int x)
{
    for (int i = 1; i < HEIGHT - 1; i++)
    {
        mvprintw(i, x, "|");
    }
}

void Display::drawUnusedPile()
{
    int start_x = 2;
    int y = 2;

    int hiddenCount = this->game->getBoard()->getRemainingUnusedCardCount();
    Card* currCard = this->game->getBoard()->getCurrentUnusedCard();
    if (currCard == nullptr)
    {
        drawCard(start_x, y, hiddenCount, 0, nullptr);
    }
    else
    {
        Card* nextCard = this->game->getBoard()->getNextUnusedCard();
        if (nextCard == nullptr)
        {   
            drawCardDivider(start_x, y++, true);
            mvprintw(y++, start_x, "| X |");
            drawCardDivider(start_x, y++, false);
            mvprintw(y++, start_x, "|%s%s|", getValueChar(currCard->getValue()).c_str(), getSuitChar(currCard->getSuit()).c_str());
        }
        else
        {
            Card** visibleCards = new Card*[1];
            visibleCards[0] = currCard;
            drawCard(start_x, y++, hiddenCount, 1, &visibleCards);
            delete[] visibleCards;
        }
        drawCardDivider(start_x, y, true);
    }
}

void Display::drawStack(int stackIndex)
{
    int start_x = 12 + 7 * stackIndex;
    int start_y = 2;

    int hiddenCount = 0;
    int visibleCount = 0;
    Card* stack[this->game->getBoard()->getStackLength(stackIndex)];

    for (int i = 0; i < this->game->getBoard()->getStackLength(stackIndex); i++)
    {
        stack[i] = this->game->getBoard()->getCardFromStack(stackIndex, i);
        stack[i]->getIsFaceUp() ? visibleCount++ : hiddenCount++;
    }
    Card** visibleStack = new Card*[visibleCount];
    for (int i = 0; i < visibleCount; i++)
    {
        visibleStack[i] = stack[i + hiddenCount];
    }
    drawCard(start_x, start_y, hiddenCount, visibleCount, &visibleStack);
    delete[] visibleStack;
}

void Display::drawFoundation(Suit suitIndex)
{
    int foundationLength = this->game->getBoard()->getFoundationLength(suitIndex);
    int start_x = 64 + 7 * (this->use2ColFoundation ? suitIndex % 2 : 0);
    int start_y = 2 + 4 * (this->use2ColFoundation ? suitIndex / 2 : suitIndex);

    if (foundationLength == 0)
    {   
        drawCardDivider(start_x, start_y, true);
        mvprintw(start_y + 1, start_x, "| %s |", getSuitChar(suitIndex).c_str());
        drawCardDivider(start_x, start_y + 2, true);
    }
    else
    {
        // Make a 1-element array to pass to drawCard
        Card* foundationCard = foundationLength == 0 ? nullptr : this->game->getBoard()->getCardFromFoundation(suitIndex, foundationLength - 1);
        Card** foundationCardArray = new Card*[1];
        foundationCardArray[0] = foundationCard;

        drawCard(start_x, start_y, 0, foundationLength, &foundationCardArray);
        delete[] foundationCardArray;
    }
}

void Display::drawCursor()
{
    int baseX = HORIZ_CURSOR_XPOS[this->horizCursorXIndex];
    CursorPileInfo& cursorPile = this->pileCursors[this->horizCursorXIndex];
    int yPos;

    if (this->horizCursorXIndex <= STACK_COUNT)
    {
        mvprintw(cursorPile.startingY - 1, baseX + 2, "vvv");
        mvprintw(cursorPile.startingY + cursorPile.yHeight, baseX + 2, "^^^");
    }

    if (this->horizCursorXIndex == 0) // Unused Pile
    {   
        yPos = cursorPile.startingY + 1 + 2 * cursorPile.currentCursorVerticalIndex;
    }
    else if (this->horizCursorXIndex <= STACK_COUNT) // Stacks
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
        mvprintw(yPos - 2, baseX + 2, "vvv");
        mvprintw(yPos + 2, baseX + 2, "^^^");
    }
    // We will want to use pileCursors to check the height of the stack and print the cursor at the appropriate position
    mvprintw(yPos, baseX, ">");
    mvprintw(yPos, baseX + 6, "<");
}

void Display::drawCard(int start_x, int start_y, int hiddenCount, int visibleCount, Card** cards[])
{   
    int current_y = start_y;
    drawCardDivider(start_x, current_y++, true);

    // Draw the hidden cards
    string hiddenText = "";
    if (hiddenCount > 0)
    {
        if (hiddenCount == 1)
        {
            hiddenText = "| ? |";
        }
        else if (hiddenCount < 10)
        {
            hiddenText = "|?x" + std::to_string(hiddenCount) + "|";
        }
        else
        {
            hiddenText = "|" + std::to_string(hiddenCount) + "?|";
        }
        mvprintw(current_y++, start_x, hiddenText.c_str());
        drawCardDivider(start_x, current_y++, visibleCount <= 0);
    }
    for (int i = 0; i < visibleCount; i++)
    {
        Card* card = *cards[i];
        int cardValue = card->getValue();
        Suit cardSuit = card->getSuit();
        
        mvprintw(current_y++, start_x, "|%s%s|", (getValueChar(cardValue) + (cardValue == 10 ? "" : " ")).c_str(), getSuitChar(cardSuit).c_str());
    }
    if (visibleCount > 0)
    {
        drawCardDivider(start_x, current_y++, true);
    }
};

void Display::drawCardDivider(int x, int y, bool isEdge)
{   
    mvprintw(y, x, "+%s+", isEdge ? "===" : "---");
};

string Display::getSuitChar(Suit suit)
{   
    if (useUnicode)
    {    
        switch (suit)
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
            return "";
        }
    }
    else
    {
        switch (suit)
        {
        case Suit::DIAMONDS:
            return "D";
        case Suit::CLUBS:
            return "C";
        case Suit::HEARTS:
            return "H";
        case Suit::SPADES:
            return "S";
        default:
            return "";
        }
    
    }
}

string Display::getValueChar(int value)
{
    switch (value)
    {
    case 1:
        return "A";
    case 11:
        return "J";
    case 12:
        return "Q";
    case 13:
        return "K";
    default:
        return std::to_string(value);
    }
}