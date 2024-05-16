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
    char* saveData = writeSaveData();
    std::ofstream saveFile(SAVEFILE_NAME);
    if (saveFile.is_open())
    {
        saveFile << saveData;
        saveFile.close();
        delete[] saveData;
        return true;
    }
    else // Error saving the file, Maybe print error message at display
    {
        delete[] saveData;
        return false;
    }
}

bool Persistence::loadFile()
{
    // Load the game
    return false;
}

/* Save the game in the format
Stack Piles SEP Foundation Piles SEP UnusedCount[1] Unused Pile SEP Move Count[2]

For each pile, the format is a char (8 bits)
Signed bit: Positive for face up, Negative for face down
Value bits: 0-51 (13 * suitIndex + (value - 1))
Range: [0-51] U [128-179]

SEP: 255/-1

Maximum size: 52 Cards * 1 byte char + 9 new lines * 1 byte (char sep, -128) + 1 byte unusedCardIndex + 2 bytes for move count = 63 bytes
Note that if foundation piles are filled (like up to 5H) then we can omit 4 cards (AH-4H)
*/
char* Persistence::writeSaveData()
{
    // Make a new char array to store the save data
    int saveDataLength = getArrayLength();
    char* saveData = new char[saveDataLength]; // Read bottom comment for the size calculation
    int saveDataIndex = 0;

    for (int i = 0; i < STACK_COUNT; i++)
    {
        // Save the stack data
        writeStackData(i, saveData, &saveDataIndex);
        writeSep(saveData, &saveDataIndex);
    }

    // For some reason, the output file only writes up to stack piles...

    for (int i = 0; i < FOUNDATION_COUNT; i++)
    {
        // Save the foundation data
        writeFoundationData(i, saveData, &saveDataIndex);
        writeSep(saveData, &saveDataIndex);
    }

    // Save remaining unused cards count
    saveData[saveDataIndex++] = this->board->getRemainingUnusedCardCount() & 0xFF;

    // Save the unused pile data
    writeUnusedData(saveData, &saveDataIndex);
    writeSep(saveData, &saveDataIndex);

    // Clamp moves to 2 bytes
    int moveCount = this->board->getMoves();
    saveData[saveDataIndex++] = (moveCount >> 8) & 0xFF;
    saveData[saveDataIndex++] = moveCount & 0xFF;

    return saveData;
}

void Persistence::writeStackData(int stackIndex, char* saveData, int* saveDataIndex)
{
    int stackLength = this->board->getStackLength(stackIndex);
    for (int i = 0; i < stackLength; i++)
    {
        Card* card = this->board->getCardFromStack(stackIndex, i);
        writeCardData(card, saveData, saveDataIndex);
    }
}

void Persistence::writeFoundationData(int suitIndex, char* saveData, int* saveDataIndex)
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

void Persistence::writeUnusedData(char* saveData, int* saveDataIndex)
{
    Card* currCard = this->board->getCurrentUnusedCard();
    Card* savedCard = currCard;
    do {
        writeCardData(currCard, saveData, saveDataIndex);
        currCard = this->board->shiftNextUnusedCard();
    }
    while (currCard != savedCard);
}

void Persistence::writeSep(char* saveData, int* saveDataIndex)
{
    saveData[*saveDataIndex] = -1;
    *saveDataIndex += 1;
}

void Persistence::writeCardData(Card* card, char* saveData, int* saveDataIndex)
{
    // Save the card data
    if (card == nullptr)
    {
        return;
    }
    int cardData = static_cast<int>(card->getSuit()) * MAX_VALUE + card->getValue() - 1;
    if (card->getIsFaceUp())
    {
        saveData[*saveDataIndex] = cardData;
    }
    else
    {
        saveData[*saveDataIndex] = cardData + 128; // Set the sign bit to 1
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
    return MAX_CARDS - omittedCount + LINE_BREAK_COUNT + 1 + 2;
}