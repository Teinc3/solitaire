#include <iostream>
#include <string>
#include <curses.h>

using std::string;

template<typename... Args>
string formatString(const string& format, Args... args) {
    std::ostringstream ss;
    using List= int[];
    (void)List{0, ((void)(ss << args), 0)...};

    string s = ss.str();
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '%') {
            s.replace(i, 2, format);
            i += format.size() - 1;
        }
    }

    return s;
}

int main() {
    initscr();  // Initialize the window
    
    getch();  // Wait for user input

    endwin();  // End curses mode
    return 0;
}