#include "common.hpp"

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