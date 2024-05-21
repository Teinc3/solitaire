#include "info.hpp"

Info::Info()
{
    this->topRowIndex = 0;
    this->prevMenuState = GameState::MAIN_MENU;

    parseInfoText();
}

void Info::shiftRow(bool isDown)
{
    if (isDown)
    {
        if (this->topRowIndex + HEIGHT - 7 < this->rowCount - 1)
        {
            this->topRowIndex++;
        }
    }
    else
    {
        if (this->topRowIndex > 0)
        {
            this->topRowIndex--;
        }
    }
}

void Info::render()
{
    // Print the info text
    string text = string(this->infoText);
    // Skip rows that are not visible
    for (int i = 0; i < this->topRowIndex; i++)
    {
        text = text.substr(text.find("\n") + 1);
    }

    for (int i = 2; i < HEIGHT - 3; i++)
    {
        // Print the text
        int newLineIndex = text.find("\n");
        string line = text.substr(0, newLineIndex);
        monoColorPrint(ColorPair::BLUE, i, 2, line);
        text = text.substr(newLineIndex + 1);
    }
}

GameState Info::getPrevMenuState()
{
    return this->prevMenuState;
}

void Info::setPrevMenuState(GameState state)
{
    this->prevMenuState = state;
}

void Info::parseInfoText()
{
    // Count the number of rows
    this->rowCount = 0;
    this->infoText = "";

    addSectionText(0);
    addHorizontalDelimiter();
    addSectionText(1);
    addHorizontalDelimiter();
    addSectionText(2);

    // Remove the last newline character
    this->infoText = this->infoText.substr(0, this->infoText.length() - 1);
    this->rowCount--;
}

void Info::addSectionText(int sectionIndex)
{
    for (int i = 0; i < SECTION_LENGTH[sectionIndex]; i++)
    {
        string text = (sectionIndex == 0) ? TUTORIAL[i] : (sectionIndex == 1) ? CONTROLS[i] : ABOUT[i];

        // For Header we center it
        if (i == 0 || sectionIndex == 2)
        {
            // Center the text
            int padding = (MAX_INFO_WIDTH - text.length()) / 2;
            for (int j = 0; j < padding; j++)
            {
                this->infoText += " ";
            }
            this->infoText += string(text) + "\n";
            this->rowCount++;
            
            if (i == 0)
            {
                this->infoText += "\n";
                this->rowCount++;
            }
            continue;
        }

        // Split the text into multiple lines if it exceeds the width
        for (size_t j = 0; j < text.length(); j += MAX_INFO_WIDTH)
        {
            this->infoText += string(text.substr(j, MAX_INFO_WIDTH)) + "\n";
            this->rowCount++;
        }
    }
}

void Info::addHorizontalDelimiter()
{
    this->infoText += "\n";
    for (int i = 0; i < MAX_INFO_WIDTH; i++)
    {
        this->infoText += "*";
    }
    this->infoText += "\n\n";
    this->rowCount += 3;
}