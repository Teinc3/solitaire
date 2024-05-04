#include <curses.h>

int main() {
    initscr();  // Initialize the window
    keypad(stdscr, TRUE);  // Enable special keys

    int ch;
    while ((ch = getch()) != 'q') {  // Press 'q' to quit
        switch (ch) {
        case KEY_UP:
            printw("Up arrow key pressed\n");
            break;
        case KEY_DOWN:
            printw("Down arrow key pressed\n");
            break;
        case KEY_LEFT:
            printw("Left arrow key pressed\n");
            break;
        case KEY_RIGHT:
            printw("Right arrow key pressed\n");
            break;
        default:
            printw("Key code: %d\n", ch);
            break;
        }
        refresh();  // Update the window
    }

    endwin();  // End curses mode
    return 0;
}