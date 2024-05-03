#include "display.hpp"

Display::Display(Game* game)
{   
    initscr();  // Initialize the library
    cbreak();   // Line buffering disabled
    noecho();   // Don't echo() while we do getch

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
            exit(1);
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
    case GameState::GAME_MENU:
        drawMenu(true);
        // Falls through next case so that the game board is also drawn
    case GameState::PLAYING:
        // Draw the game board
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
                mvprintw(i, j, "*");  // Print # on the top and bottom
            }
            else if (j == 0 || j == this->width - 1)
            {
                mvprintw(i, j, "#");  // Print * on the sides
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