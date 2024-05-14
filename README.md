# Solitaire
by Teinc3

## Description
A simple solitaire game written in C++ that can be played in the terminal.

This game was built solo to inch (mock) freeriders for being completely useless in a group project (Course: COMP2113/ENGG1340)

Although there are insane amounts of antipatterns in the code... So I guess I'm not doing much better.

## Todo
- Complete `Persistence` class for saving and loading games
- Change color function to allow multiple colors within a string 
- Add information page to the game, explaining instructions and controls

## How to play
- Build the game using **makefile** or download the executable from the releases
- Run the game using `./bin/solitaire` or double-click the executable

## Controls
- Arrow keys to move the cursor
- Enter key to stage/commit a move
- Use the Backspace or Delete key to open/close the menu

## Specifications
- Minimum terminal size: 69x22 (The game will refuse to launch if the terminal is too small)

## Dependencies
### Windows Powershell:
- `pdcurses`
### MacOS/Linux:
- `ncurses`