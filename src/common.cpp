#include "common.hpp"

void monoColorPrint(ColorPair colorPair, int y, int startingX, string text)
{
    int color = static_cast<int>(colorPair);
    attron(COLOR_PAIR(color));
    mvprintw(y, startingX, text.c_str());
    attroff(COLOR_PAIR(color));
}

void multiColorPrint(int y, int startingX, string text, int colorCount, ColorRange* colorRanges)
{
    int charIndex = 0;
    for (int colorRangeIndex = 0; colorRangeIndex < colorCount; colorRangeIndex++)
    {
        int color = static_cast<int>(colorRanges[colorRangeIndex].color);
        attron(COLOR_PAIR(color));

        // Print text of the current color range
        for (; charIndex < colorRanges[colorRangeIndex].end; charIndex++)
        {
            mvaddch(y, startingX++, text[charIndex]);
        }
        
        attroff(COLOR_PAIR(color));
    }
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

bool isRed(Suit suit)
{
    return suit == Suit::DIAMONDS || suit == Suit::HEARTS;
}