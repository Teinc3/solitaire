#pragma once

#include <common.hpp>
#include <game.hpp>

const int MIN_WIDTH = 71; //1+1+5+2+1+(2+5)*7+2+1+2+5+1+1
const int HEIGHT = 21; //1+1+(3+13+1)+1+1
const int MIN_2COL_FOUNDATION_WIDTH = 78; //MIN_WIDTH+(2+5)

class Display {
public:
    Display(Game*);
    ~Display();

    void render();

private:
    int width;
    bool use2ColFoundation;

    Game* game;

    void drawBoundary();
    void drawMenu(bool);
};