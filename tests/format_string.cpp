#include <iostream>
#include <string>
#include <sstream>

using std::string;

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

int main(void)
{
    string args[] = { "Alice", "3" };
    string s = formatString("Hello, ~! You have ~ apples.", 2, args);
    std::cout << s << std::endl;

    return 0;
}