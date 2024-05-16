#include "persistence.hpp"

Persistence::Persistence(Board* board)
{
    this->board = board;
}

Persistence::~Persistence()
{
    this->board = nullptr;
}

bool Persistence::saveFile()
{
    char* saveData = generateSaveData();
    std::ofstream saveFile(SAVEFILE_NAME);
    if (saveFile.is_open())
    {
        saveFile << saveData;
        saveFile.close();
        return true;
    }
    else // Error saving the file, Maybe print error message at display
    {
        return false;
    }
}

bool Persistence::loadFile()
{
    // Load the game
}

/* Save the game in the format
Line 1-7: Stack Piles
Line 8: Foundation Piles
Line 9: Unused Pile
Line 10: Move Count

For each pile, the format is a char (8 bits)
Signed bit: Positive for face up, Negative for face down
Value bits: 0-51 (13 * suitIndex + (value - 1))

Maximum size: 52 Cards * 1 byte + 9 new lines * 1 byte (\n) + 2 bytes for move count + 1 byte for null terminator = 64 bytes
Note that if foundation piles are filled (like up to 5H) then we can omit 4 cards (AH-4H)
*/
char* Persistence::generateSaveData()
{
    // Make a new char array to store the save data
    int saveDataLength = getArrayLength();
    char saveData[saveDataLength]; // Read bottom comment for the size calculation
    int saveDataIndex = 0;

    for (int i = 0; i < STACK_COUNT; i++)
    {
        // Save the stack data
        generateStackData(i, saveData, &saveDataIndex);
        generateNewLine(saveData, &saveDataIndex);
    }

    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        // Save the foundation data
        generateFoundationData(i, saveData, &saveDataIndex);
        generateNewLine(saveData, &saveDataIndex);
    }

    // Save the unused pile data
    generateUnusedData(saveData, &saveDataIndex);
    generateNewLine(saveData, &saveDataIndex);

    // Clamp moves to 2 bytes
    int moveCount = this->board->getMoves();
    saveData[saveDataIndex++] = (moveCount >> 8) & 0xFF;
    saveData[saveDataIndex++] = moveCount & 0xFF;

    // Save null terminator
    saveData[saveDataIndex] = '\0';

    return saveData;
}

void Persistence::generateStackData(int stackIndex, char* saveData, int* saveDataIndex)
{
    int stackLength = this->board->getStackLength(stackIndex);
    for (int i = 0; i < stackLength; i++)
    {
        Card* card = this->board->getCardFromStack(stackIndex, i);
        writeCardData(card, saveData, saveDataIndex);
    }
}

void Persistence::generateFoundationData(int suitIndex, char* saveData, int* saveDataIndex)
{
    // Basically just find the topmost visible card in the foundation and save it
    int foundationLength = this->board->getFoundationLength(suitIndex);
    if (foundationLength > 0)
    {
        Card* card = this->board->getCardFromFoundation(suitIndex, foundationLength - 1);
        writeCardData(card, saveData, saveDataIndex);
    }
    // No need to save empty foundation piles since we already encode the suit in the foundation data
}

void Persistence::generateUnusedData(char* saveData, int* saveDataIndex)
{
    Card* currCard = this->board->getCurrentUnusedCard();
    Card* savedCard = currCard;
    do {
        writeCardData(currCard, saveData, saveDataIndex);
        currCard = this->board->shiftNextUnusedCard();
    }
    while (currCard != savedCard);
}

void Persistence::generateNewLine(char* saveData, int* saveDataIndex)
{
    saveData[*saveDataIndex] = '\n';
    *saveDataIndex += 1;
}

void Persistence::writeCardData(Card* card, char* saveData, int* saveDataIndex)
{
    // Save the card data
    if (card == nullptr)
    {
        return;
    }
    char cardData = card->getSuit() * 13 + card->getValue() - 1;
    if (card->getIsFaceUp())
    {
        saveData[*saveDataIndex] = cardData;
    }
    else
    {
        saveData[*saveDataIndex] = -cardData;
    }
    *saveDataIndex += 1;
}

int Persistence::getArrayLength()
{
    int omittedCount = 0;
    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        int foundationLength = this->board->getFoundationLength(i);
        if (foundationLength > 0)
        {
            omittedCount += foundationLength - 1;
        }
    }
    return MAX_CARDS - omittedCount + LINE_BREAK_COUNT + 2 + 1; // Moves + null terminator
}