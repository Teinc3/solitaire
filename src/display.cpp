#include "display.hpp"
#include "board.hpp"

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
        std::cout << std::endl << "Please increase the size of your terminal to at least " << MIN_WIDTH << "x" << HEIGHT << "." << std::endl;
        return;
    }

    this->game = game;
    this->cursor = new Cursor(this->game->getBoard());
    
    onNewGame();
}

Display::~Display()
{
    clear();
    refresh();
    endwin();  // End curses mode

    delete this->cursor;

    this->game = nullptr;
    this->cursor = nullptr;
}

void Display::onNewGame()
{
    this->currentMessageIndex = 0;

    this->cursor->onNewGame();
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

Cursor* Display::getCursor()
{
    return this->cursor;
}

void Display::setMessage(int messageIndex)
{
    this->currentMessageIndex = messageIndex;
}

bool Display::resetMessage(bool isBackspace)
{
    if (this->currentMessageIndex == 0 || (!isBackspace && this->currentMessageIndex != 3))
    {
        return false;
    }

    this->currentMessageIndex = 0;
    if (!isBackspace) // Auto finish
    {
        this->game->finishGame();
    }
    return true;
}

void Display::drawBoundary()
{

    for (int i = 0; i < HEIGHT - 1; i++)
    {
        for (int j = 0; j < MIN_WIDTH; j++)
        {
            if (i == 0 || i == HEIGHT - 2 || j == 0 || j == MIN_WIDTH - 1)
            {
                coloredPrint(GREEN, i, j, "#");  // Print # at the boundary
            }
        }
    }
}

void Display::drawMenu(bool isGameMenu) {
    int start_y = (HEIGHT - 10) / 2;  // Calculate the starting y position
    int start_x = (MIN_WIDTH - 27) / 2;  // Calculate the starting x position

    MenuOption menuOption = this->game->getMenuOption();
    bool menuOptions[4] = { menuOption == MenuOption::NEW_GAME, menuOption == MenuOption::LOAD_SAVE_GAME, menuOption == MenuOption::INFO, menuOption == MenuOption::QUIT };
    string arg1[3] = { menuOptions[0] ? ">" : " ", isGameMenu && !this->game->getHasAlreadyWon() ? "Continue" : "New Game", menuOptions[0] ? "<" : " " };
    string arg2[3] = { menuOptions[1] ? ">" : " ", isGameMenu ? "Save" : "Load", menuOptions[1] ? "<" : " " };
    string arg3[2] = { menuOptions[2] ? ">" : " ", menuOptions[2] ? "<" : " " };
    string arg4[3] = { menuOptions[3] ? ">" : " ", isGameMenu ? "Stop" : "Quit", menuOptions[3] ? "<" : " " };
    
    coloredPrint(BLACK, start_y + 0, start_x, "+=========================+");
    coloredPrint(RED, start_y + 1, start_x, "|        SOLITAIRE        |");
    coloredPrint(RED, start_y + 2, start_x, "|        by Teinc3        |");
    coloredPrint(BLACK, start_y + 3, start_x, "+-------------------------+");
    coloredPrint(menuOptions[0] ? YELLOW : BLUE, start_y + 4, start_x, formatString("|     ~ 1. ~ ~     |", 3, arg1));
    coloredPrint(menuOptions[1] ? YELLOW : BLUE, start_y + 5, start_x, formatString("|    ~ 2. ~ Game ~     |", 3, arg2));
    coloredPrint(menuOptions[2] ? YELLOW : BLUE, start_y + 6, start_x, formatString("|   ~ 3. Information ~    |", 2, arg3));
    coloredPrint(menuOptions[3] ? YELLOW : BLUE, start_y + 7, start_x, formatString("|    ~ 4. ~ Game ~     |", 3, arg4));
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

    this->cursor->drawCursor();
    drawMessage();
}

void Display::drawDelimiter(int x)
{
    for (int i = 1; i < HEIGHT - 2; i++)
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
        if (hiddenCount > 0)
        {
            drawCard(start_x, y, hiddenCount, 0, nullptr);
        }
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
    int start_x = HORIZ_CURSOR_XPOS[8] + 1 + COL_WIDTH * (suitIndex % 2);
    int start_y = 2 + 4 * (suitIndex / 2);

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

void Display::drawMessage()
{
    int y = HEIGHT - 1;
    // Draw moves at x = 62
    int moves = this->game->getBoard()->getMoves();
    string message = "Moves: " + std::to_string(moves);

    coloredPrint(BLACK, y, MOVE_MSG_STARTING_X, message);

    // Draw the message (if any)
    if (this->currentMessageIndex == 1)
    {
        coloredPrint(YELLOW, y, MSG_STARTING_X, string(MESSAGES[1]) + string(MESSAGES[2]));
    }
    else if (this->currentMessageIndex == 3)
    {
        coloredPrint(YELLOW, y, MSG_STARTING_X, string(MESSAGES[3]));
    }
    else if (this->currentMessageIndex >= 4) // Errors
    {
        coloredPrint(RED, y, MSG_STARTING_X, string(MESSAGES[4]) + string(MESSAGES[2]));
    }
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