#include "game.hpp"

int main()
{
    Game* game = new Game();
    
    while (game->getIsRunning())
    {
        game->update();
    }

    std::cout << "Closing Game..." << std::endl;
    delete game;
    game = nullptr;

    return 0;
}