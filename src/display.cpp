#include "display.hpp"
#include "board.hpp"

void coloredPrint(int colorPair, int y, int x, string text)
{
    attron(COLOR_PAIR(colorPair));
    mvprintw(y, x, text.c_str());
    attroff(COLOR_PAIR(colorPair));
}

string formatString(string format, size_t argc, string argv[])
{
    // For every "%" in the format string, replace it with the next argument (if available)
    std::ostringstream ss;
    size_t argIndex = 0;
    for (size_t i = 0; i < format.size(); i++)
    {
        if (format[i] == '~' && argIndex < argc)
        {
            ss << argv[argIndex];
            argIndex++;
        }
        else
        {
            ss << format[i];
        }
    }

    return ss.str();
}

Display::Display(Game* game)
{   
    initscr();
    keypad(stdscr, TRUE);  // Enable special keys
    cbreak();
    noecho();
    curs_set(0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Default text color
    init_pair(RED, COLOR_RED, COLOR_BLACK); // Red text color
    init_pair(BLACK, COLOR_WHITE, COLOR_BLACK); // White text color
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK); // Yellow text color
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK); // Blue text color

    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);  // Get the size of the window

    if (max_x < MIN_WIDTH || max_y < HEIGHT)
    {
        // quit ncurses and print an error message
        endwin();
        game->setIsRunning(false);
        std::cout << std::endl << "Please increase the size of your terminal to at least" << MIN_WIDTH << "x" << HEIGHT << "." << std::endl;
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
    this->lockedCursorPileIndex = this->horizCursorXIndex;
    this->isLockedCursor = false;
}

Display::~Display()
{
    clear();
    refresh();
    endwin();  // End curses mode

    this->game = nullptr;
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
    if (!this->isLockedCursor)
    {
        updateCursorLock(false);
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

void Display::updateCursorLock(bool changeCursorStatus)
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

int Display::getHorizCursorXIndex()
{
    return this->horizCursorXIndex;
}

int Display::getVerticalCursorIndex()
{
    return this->pileCursors[this->horizCursorXIndex].currentCursorVerticalIndex;
}

int Display::getLockedCursorPileIndex()
{
    return this->lockedCursorPileIndex;
}

int Display::is2ColFoundation()
{
    return this->use2ColFoundation;
}

void Display::drawBoundary()
{

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == this->width - 1)
            {
                coloredPrint(GREEN, i, j, "#");  // Print # at the boundary
            }
        }
    }
}

void Display::drawMenu(bool isGameMenu) {
    int start_y = (HEIGHT - 9) / 2;  // Calculate the starting y position
    int start_x = (this->width - 27) / 2;  // Calculate the starting x position

    MenuOption menuOption = this->game->getMenuOption();
    bool menuOptions[4] = { menuOption == MenuOption::NEW_GAME, menuOption == MenuOption::LOAD_SAVE_GAME, menuOption == MenuOption::INFO, menuOption == MenuOption::QUIT };
    string arg1[2] = { menuOptions[0] ? ">" : " ", menuOptions[0] ? "<" : " " };
    string arg2[3] = { menuOptions[1] ? ">" : " ", isGameMenu ? "Save" : "Load", menuOptions[1] ? "<" : " " };
    string arg3[2] = { menuOptions[2] ? ">" : " ", menuOptions[2] ? "<" : " " };
    string arg4[3] = { menuOptions[3] ? ">" : " ", isGameMenu ? "Main Menu" : "Quit Game", menuOptions[3] ? "<" : " " };
    
    coloredPrint(BLACK, start_y + 0, start_x, "+=========================+");
    coloredPrint(RED, start_y + 1, start_x, "|        SOLITAIRE        |");
    coloredPrint(RED, start_y + 2, start_x, "|        by Teinc3        |");
    coloredPrint(BLACK, start_y + 3, start_x, "+-------------------------+");
    coloredPrint(menuOptions[0] ? YELLOW : BLUE, start_y + 4, start_x, formatString("|     ~ 1. New Game ~     |", 2, arg1));
    coloredPrint(menuOptions[1] ? YELLOW : BLUE, start_y + 5, start_x, formatString("|    ~ 2. ~ Game ~     |", 3, arg2));
    coloredPrint(menuOptions[2] ? YELLOW : BLUE, start_y + 6, start_x, formatString("|   ~ 3. Information ~    |", 2, arg3));
    coloredPrint(menuOptions[3] ? YELLOW : BLUE, start_y + 7, start_x, formatString("|    ~ 4. ~ ~     |", 3, arg4));
    coloredPrint(BLACK, start_y + 8, start_x, "+=========================+");

    refresh();  // Refresh the screen to show the menu
}

// Reference docs/game_design.txt for design
void Display::drawGameBoard()
{
    drawDelimiter(9);
    drawDelimiter(59);

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
        coloredPrint(GREEN, i, x, "|");
    }
}

void Display::drawUnusedPile()
{
    int start_x = HORIZ_CURSOR_XPOS[0] + 1;
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
            coloredPrint(GREEN, y++, start_x, "| X |");
            drawCardDivider(start_x, y++, false);

            string args[3] = { getValueChar(currCard->getValue()).c_str(), currCard->getValue() == 10 ? "" : " ", getSuitChar(currCard->getSuit()).c_str() };
            coloredPrint(currCard->getIsRed() ? RED : BLACK, y++, start_x, formatString("|~~~|", 3, args));
            
            drawCardDivider(start_x, y, true);
        }
        else
        {
            Card* visibleCards[1] { currCard };
            y = drawCard(start_x, y++, hiddenCount, 1, visibleCards);
        }
    }
}

void Display::drawStack(int stackIndex)
{
    int stackLength = this->game->getBoard()->getStackLength(stackIndex);
    if (stackLength == 0)
    {
        return;
    }

    int hiddenCount = 0;
    int visibleCount = 0;
    int start_x = HORIZ_CURSOR_XPOS[1] + 1 + COL_WIDTH * stackIndex;
    int start_y = 2;

    Card* stack[stackLength];

    for (int i = 0; i < stackLength; i++)
    {
        stack[i] = this->game->getBoard()->getCardFromStack(stackIndex, i);
        stack[i]->getIsFaceUp() ? visibleCount++ : hiddenCount++;
    }
    Card* visibleStack[visibleCount];
    for (int i = 0; i < visibleCount; i++)
    {
        visibleStack[i] = stack[i + hiddenCount];
    }
    drawCard(start_x, start_y, hiddenCount, visibleCount, visibleStack);
}

void Display::drawFoundation(Suit suitIndex)
{
    int foundationLength = this->game->getBoard()->getFoundationLength(suitIndex);
    int start_x = HORIZ_CURSOR_XPOS[8] + 1 + COL_WIDTH * (this->use2ColFoundation ? suitIndex % 2 : 0);
    int start_y = 2 + 4 * (this->use2ColFoundation ? suitIndex / 2 : suitIndex);

    if (foundationLength == 0)
    {   
        drawCardDivider(start_x, start_y, true);
        string args[1] = { getSuitChar(suitIndex).c_str() };
        coloredPrint(suitIndex % 2 == 0 ? RED : BLACK, start_y + 1, start_x, formatString("| ~ |", 1, args));
        drawCardDivider(start_x, start_y + 2, true);
    }
    else
    {
        // Make a 1-element array to pass to draw-card
        Card* foundationCard = foundationLength == 0 ? nullptr : this->game->getBoard()->getCardFromFoundation(suitIndex, foundationLength - 1);
        Card* foundationCardArray[1] { foundationCard };

        drawCard(start_x, start_y, 0, foundationLength >= 1 ? 1 : 0, foundationCardArray);
    }
}

void Display::drawCursor()
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

int Display::drawCard(int start_x, int start_y, int hiddenCount, int visibleCount, Card* cards[])
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
        coloredPrint(GREEN, current_y++, start_x, hiddenText.c_str());
        drawCardDivider(start_x, current_y++, visibleCount <= 0);
    }
    for (int i = 0; i < visibleCount; i++)
    {
        Card* card = cards[i];
        int cardValue = card->getValue();
        Suit cardSuit = card->getSuit();

        string args[2] = { (getValueChar(cardValue) + (cardValue == 10 ? "" : " ")).c_str(), getSuitChar(cardSuit).c_str() };
        coloredPrint(card->getIsRed() ? RED : BLACK, current_y++, start_x, formatString("|~~|", 2, args));
    }
    if (visibleCount > 0)
    {
        drawCardDivider(start_x, current_y++, true);
    }

    return current_y;
};

void Display::drawCardDivider(int x, int y, bool isEdge)
{   
    string args[1] = { isEdge ? "===" : "---" };
    coloredPrint(GREEN, y, x, formatString("+~+", 1, args));
};

string Display::getSuitChar(Suit suit)
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

