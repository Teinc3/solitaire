# Solitaire
by Teinc3

## Description
A simple solitaire game written in C++ that can be played in the terminal.

## Todo
- Change color function to allow multiple colors within a string
- Release executables for Windows and Unix

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