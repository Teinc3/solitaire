#include "game.hpp"

int main()
{
    Game game;
    while (game.getIsRunning())
    {
        game.update();
    }
    game.~Game();
    return 0;
}