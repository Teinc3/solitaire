#include "common.hpp"
#include "game.hpp"
#include "display.hpp"

int main()
{
    Game game;
    
    Display* display = game.getDisplay();
    display->render(); // First render - delete if want animations and unblocking input

    while (game.getIsRunning())
    {
        game.handleInput();
        game.update();
        display->render();
    }
    
    return 0;
}