#include "display.hpp"

Display::Display(Game* game)
{   
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);  // Get the size of the window

    if (max_x < MIN_WIDTH || max_y < HEIGHT)
    {
        // Try to resize the terminal
        if (resizeterm(HEIGHT, MIN_WIDTH) == ERR)
        {
            // If resizing is not possible, quit ncurses and print an error message
            endwin();
            std::cout << "Terminal size is too small. Please increase the size of your terminal." << std::endl;
            game->setIsRunning(false);
            return;
        }
    }
    else if (max_x < MIN_2COL_FOUNDATION_WIDTH)
    {
        use2ColFoundation = false;
    }
    else
    {
        use2ColFoundation = true;
    }
    this->width = use2ColFoundation ? MIN_2COL_FOUNDATION_WIDTH : MIN_WIDTH;

    this->game = game;
    this->useUnicode = false;
}

Display::~Display()
{
    endwin();  // End curses mode
}

void Display::render()
{
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
    for (int i = 0; i < HEIGHT; i++)
    {
        mvprintw(i, x, "|");
    }
}

void Display::drawUnusedPile()
{
    //drawCard(2, 2, this->game->getBoard()->getUnusedCards().size(), 1, nullptr);
}

void Display::drawStack(int stackIndex)
{
    
}

void Display::drawFoundation(Suit suitIndex)
{
    int foundationLength = this->game->getBoard()->getFoundationLength(suitIndex);
    int start_x = 64 + 7 * (suitIndex - (use2ColFoundation && suitIndex > 1 ? 2 : 0));
    int start_y = use2ColFoundation && suitIndex > 1 ? 6 : 2;

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
        drawCardDivider(start_x, current_y++, false);
    }
    for (int i = 0; i < visibleCount; i++)
    {
        Card* card = *cards[i];
        int cardValue = card->getValue();
        Suit cardSuit = card->getSuit();
        
        mvprintw(current_y++, start_x, "|%s%s|", (getValueChar(cardValue) + (cardValue == 10 ? "" : " ")).c_str(), getSuitChar(cardSuit).c_str());
    }
    drawCardDivider(start_x, current_y++, true);
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