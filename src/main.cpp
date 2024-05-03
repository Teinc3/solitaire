#include "game.hpp"
#include "display.hpp"

int main()
{
    Game* game = new Game();
    
    Display* display = game->getDisplay();
    while (game->getIsRunning())
    {
        game->handleInput();
        game->update();
        display->render();
    }

    delete game;
    game = nullptr;

    return 0;
}