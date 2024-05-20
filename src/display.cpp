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
    // Ignore WHITE, as it is default
    init_pair(ColorPair::RED, COLOR_RED, COLOR_BLACK);
    init_pair(ColorPair::GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(ColorPair::BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(ColorPair::YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(ColorPair::CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(ColorPair::MAGENTA, COLOR_MAGENTA, COLOR_BLACK);

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
    this->info = new Info();
    
    onNewGame();
}

Display::~Display()
{
    clear();
    refresh();
    endwin();  // End curses mode

    delete this->cursor;
    delete this->info;

    this->game = nullptr;
    this->cursor = nullptr;
    this->info = nullptr;
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
    case GameState::GAME_MENU:
        drawMenu(true);
        break;
    case GameState::INFO_PAGE:
        this->info->render();
        break;
    case GameState::PLAYING:
        drawGameBoard();
        break;
    default:
        break;
    }

    drawMessage(this->game->getGameState() == GameState::PLAYING);

    refresh();
}

Cursor* Display::getCursor()
{
    return this->cursor;
}

Info* Display::getInfo()
{
    return this->info;
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
    for (int y = 0; y < HEIGHT - 1; y++)
    {
        for (int x = 0; x < MIN_WIDTH; x++)
        {
            if (y == 0 || y == HEIGHT - 2 || x == 0 || x == MIN_WIDTH - 1)
            {
                monoColorPrint(ColorPair::GREEN, y, x, "#");  // Print # at the boundary
            }
        }
    }
}

void Display::drawMenu(bool isGameMenu) {
    int start_y = (HEIGHT - 10) / 2;  // Calculate the starting y position
    int start_x = (MIN_WIDTH - 27) / 2;  // Calculate the starting x position
    
    string options[4] = {
        formatString("|     ~ 1. ~ ~     |", 3, (string[]){ isMenuOption(MenuOption::NEW_GAME) ? ">" : " ", isGameMenu ? "Continue" : "New Game", isMenuOption(MenuOption::NEW_GAME) ? "<" : " " }),
        formatString("|    ~ 2. ~ Game ~     |", 3, (string[]){ isMenuOption(MenuOption::LOAD_SAVE_GAME) ? ">" : " ", isGameMenu ? "Save" : "Load", isMenuOption(MenuOption::LOAD_SAVE_GAME) ? "<" : " " }),
        formatString("|   ~ 3. Information ~    |", 2, (string[]){ isMenuOption(MenuOption::INFO) ? ">" : " ", isMenuOption(MenuOption::INFO) ? "<" : " " }),
        formatString("|    ~ 4. ~ ~     |", 3, (string[]){ isMenuOption(MenuOption::QUIT) ? ">" : " ", isGameMenu ? "Quit Game" : "Leave App", isMenuOption(MenuOption::QUIT) ? "<" : " " })
    };

    monoColorPrint(ColorPair::WHITE, start_y++, start_x, "+=========================+");
    multiColorPrint(start_y++, start_x, "|        SOLITAIRE        |", 3, (ColorRange[]){ { ColorPair::WHITE, 9 }, { ColorPair::RED, 18 }, { ColorPair::WHITE, 27 } });
    multiColorPrint(start_y++, start_x, "|        by Teinc3        |", 3, (ColorRange[]){ { ColorPair::WHITE, 9 }, { ColorPair::MAGENTA, 18}, { ColorPair::WHITE, 27 } });
    monoColorPrint(ColorPair::WHITE, start_y++, start_x, "+-------------------------+");
    multiColorPrint(start_y++, start_x, options[0], 3, (ColorRange[]){ { ColorPair::WHITE, 6 }, { isMenuOption(MenuOption::NEW_GAME) ? ColorPair::YELLOW : ColorPair::CYAN, 21 }, { ColorPair::WHITE, 27 } });
    multiColorPrint(start_y++, start_x, options[1], 3, (ColorRange[]){ { ColorPair::WHITE, 5 }, { isMenuOption(MenuOption::LOAD_SAVE_GAME) ? ColorPair::YELLOW : ColorPair::CYAN, 21 }, { ColorPair::WHITE, 27 } });
    multiColorPrint(start_y++, start_x, options[2], 3, (ColorRange[]){ { ColorPair::WHITE, 4 }, { isMenuOption(MenuOption::INFO) ? ColorPair::YELLOW : ColorPair::CYAN, 22 }, { ColorPair::WHITE, 27 } });
    multiColorPrint(start_y++, start_x, options[3], 3, (ColorRange[]){ { ColorPair::WHITE, 5 }, { isMenuOption(MenuOption::QUIT) ? ColorPair::YELLOW : ColorPair::CYAN, 21 }, { ColorPair::WHITE, 27 } });
    monoColorPrint(ColorPair::WHITE, start_y++, start_x, "+=========================+");
}

// Reference docs/game_design.txt for design
void Display::drawGameBoard()
{
    drawVerticalDelimiter(9);
    drawVerticalDelimiter(59);

    drawUnusedPile();
    for (int i = 0; i < STACK_COUNT; i++)
    {
        drawStack(i);
    }
    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        drawFoundation(static_cast<Suit>(i));
    }

    this->cursor->render();
}

void Display::drawVerticalDelimiter(int x)
{
    for (int i = 1; i < HEIGHT - 2; i++)
    {
        monoColorPrint(ColorPair::GREEN, i, x, "|");
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
            multiColorPrint(y++, start_x, "| X |", 3, (ColorRange[]){ { ColorPair::GREEN, 2 }, { ColorPair::CYAN, 3}, { ColorPair::GREEN, 5} });
            drawCardDivider(start_x, y++, false);

            int cardValue = currCard->value;
            Suit cardSuit = currCard->suit;
            string text = formatString("|~~~|", 3, (string[]){ getValueChar(cardValue).c_str(), cardValue == 10 ? "" : " ", getSuitChar(cardSuit).c_str() });
            multiColorPrint(y++, start_x, text, 3, (ColorRange[]){ { ColorPair::GREEN, 1 }, { isRed(cardSuit) ? ColorPair::RED : ColorPair::WHITE, 4 }, { ColorPair::GREEN, 5 } });
            
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
        stack[i]->isFaceUp ? visibleCount++ : hiddenCount++;
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
        drawCardDivider(start_x, start_y++, true);
        multiColorPrint(start_y++, start_x, formatString("| ~ |", 1, (string[]){ getSuitChar(suitIndex).c_str() }), 3, (ColorRange[]){ { ColorPair::GREEN, 1 }, { isRed(suitIndex) ? ColorPair::RED : ColorPair::WHITE, 3 }, { ColorPair::GREEN, 5 } });
        drawCardDivider(start_x, start_y, true);
    }
    else
    {
        // Make a 1-element array to pass to draw-card
        Card* foundationCard = foundationLength == 0 ? nullptr : this->game->getBoard()->getCardFromFoundation(suitIndex, foundationLength - 1);
        Card* foundationCardArray[1] { foundationCard };

        drawCard(start_x, start_y, 0, foundationLength >= 1 ? 1 : 0, foundationCardArray);
    }
}

void Display::drawMessage(bool drawMoves)
{
    int y = HEIGHT - 1;
    
    if (drawMoves)
    {    
        // Draw moves at x = 62
        int moves = this->game->getBoard()->getMoves();
        string message = "Moves: " + std::to_string(moves);
        monoColorPrint(ColorPair::MAGENTA, y, MOVE_MSG_STARTING_X, message);
    }

    // Draw the message (if any)
    if (this->currentMessageIndex == 0)
    {
        return;
    }
    else if (this->currentMessageIndex <= 1)
    {
        monoColorPrint(ColorPair::CYAN, y, MSG_STARTING_X, string(MESSAGES[1]) + string(MESSAGES[2]));
    }
    else if (this->currentMessageIndex < LOAD_SAVE_MSG_INDEX)
    {
        monoColorPrint(ColorPair::YELLOW, y, MSG_STARTING_X, string(MESSAGES[3]));
    }
    else if (this->currentMessageIndex < ERROR_MSG_INDEX)
    {
        monoColorPrint(this->currentMessageIndex <  LOAD_SAVE_MSG_INDEX + 2 ? ColorPair::RED : ColorPair::GREEN, y, MSG_STARTING_X, string(MESSAGES[this->currentMessageIndex]) + string(MESSAGES[2]));
    }
    else // Errors
    {
        monoColorPrint(ColorPair::RED, y, MSG_STARTING_X, string(MESSAGES[ERROR_MSG_INDEX]) + string(MESSAGES[2]));
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
        multiColorPrint(current_y++, start_x, hiddenText, 3, (ColorRange[]){ { ColorPair::GREEN, 1 }, { ColorPair::CYAN, 4 }, { ColorPair::GREEN, 5 } });
        drawCardDivider(start_x, current_y++, visibleCount <= 0);
    }
    for (int i = 0; i < visibleCount; i++)
    {
        Card* card = cards[i];
        int cardValue = card->value;
        Suit cardSuit = card->suit;

        multiColorPrint(current_y++, start_x, formatString("|~~|", 2, (string[]){ (getValueChar(cardValue) + (cardValue == 10 ? "" : " ")).c_str(), getSuitChar(cardSuit).c_str() }), 3, (ColorRange[]){ { ColorPair::GREEN, 1 }, { isRed(cardSuit) ? ColorPair::RED : ColorPair::WHITE, 4 }, { ColorPair::GREEN, 5 } });
    }
    if (visibleCount > 0)
    {
        drawCardDivider(start_x, current_y++, true);
    }

    return current_y;
};

void Display::drawCardDivider(int x, int y, bool isEdge)
{   
    monoColorPrint(GREEN, y, x, formatString("+~+", 1, (string[]){ isEdge ? "===" : "---" }));
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

bool Display::isMenuOption(MenuOption menuOption)
{
    return this->game->getMenuOption() == menuOption;
}