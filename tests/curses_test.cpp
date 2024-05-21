#include <iostream>
#include <curses.h>

int main() {
    initscr();  // Initialize the window
    keypad(stdscr, TRUE);
    
    while (true)
    {
        int ch = getch();  // Wait for user input
        clear();  // Clear the screen of all previously-printed characters
        mvprintw(0, 0, "%d", ch);  // Print "Hello, World!" at the top-left corner of the screen
        refresh();  // Refresh the screen to show the changes
    }

    endwin();  // End curses mode
    return 0;
}