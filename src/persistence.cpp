#include "persistence.hpp"

/* Save the game in the format
Stack Piles SEP Foundation Piles SEP UnusedCount[1] Unused Pile SEP Move Count[2]

For each pile, the format is a char (8 bits)
Signed bit: Positive for face up, Negative for face down
Value bits: 0-51 (13 * suitIndex + (value - 1))
Range: [0-51] U [128-179]

SEP: 255/-1

Minimum size: 4 Cards * 1 byte char + 9 new lines * 1 byte (char sep, -128) + 1 byte unusedCardIndex + 2 bytes for move count = 16 bytes
Maximum size: 52 Cards * 1 byte char + 9 new lines * 1 byte (char sep, -128) + 1 byte unusedCardIndex + 2 bytes for move count = 64 bytes
Note that if foundation piles are filled (like up to 5H) then we can omit 4 cards (AH-4H)
*/

Persistence::Persistence(Board* board, Card** deck)
{
    this->board = board;
    this->deck = deck;
}

Persistence::~Persistence()
{
    this->board = nullptr;
    this->deck = nullptr;
}

bool Persistence::saveFile()
{
    std::ofstream saveFile(SAVEFILE_NAME, std::ios::binary);
    if (saveFile.is_open())
    {
        int saveDataLength = getArrayLength();
        char* saveData = writeSaveData(saveDataLength);

        saveFile.write(saveData, saveDataLength);
        saveFile.close();
        delete[] saveData;

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
    std::ifstream saveFile(SAVEFILE_NAME, std::ios::binary);
    if (saveFile.is_open())
    {
        // Read the save data
        saveFile.seekg(0, saveFile.end);
        int saveDataLength = saveFile.tellg();
        saveFile.seekg(0, saveFile.beg);

        // Size must >= 16 bytes and <= 64 bytes
        if (saveDataLength < 16 || saveDataLength > 64)
        {
            return false;
        }

        char saveData[saveDataLength]; // Read bottom comment for the size calculation
        saveFile.read(saveData, saveDataLength);
        saveFile.close();

        return readSaveData(saveData, saveDataLength);
    }
    else // Error loading the file, Maybe print error message at display
    {
        return false;
    }
}

void Persistence::saveDebugInfo(string text)
{
    std::ofstream debugFile("debug.txt");
    if (debugFile.is_open())
    {
        debugFile << text;
        debugFile.close();
    }
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
    return MAX_CARDS - omittedCount + SEP_COUNT + 1 + 2;
}

char* Persistence::writeSaveData(int saveDataLength)
{
    // Make a new char array to store the save data
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
    }
    writeSep(saveData, &saveDataIndex);

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

bool Persistence::readSaveData(char* saveData, int saveDataLength)
{
    // Split the save data into separate piles at "SEP" (We should have 9 "SEP"s, reject if not)
    int sepCount = 0;
    int saveDataStartIndex = 0;
    int saveDataEndIndex = 0;
    for (; saveDataEndIndex < saveDataLength; saveDataEndIndex++)
    {
        if (saveData[saveDataEndIndex] == -1)
        {
            // Feed the sliced save data into helper read functions
            if (!((sepCount <= 6 && readStackData(saveData, saveDataStartIndex, saveDataEndIndex, sepCount)) || // Stacks
                (sepCount == 7 && readFoundationData(saveData, saveDataStartIndex, saveDataEndIndex)) || // Foundations
                (sepCount == 8 && readUnusedData(saveData, saveDataStartIndex, saveDataEndIndex)))) // Unused Pile 
            { 
                // Should never reach here
                return false;
            }
            
            saveDataStartIndex = saveDataEndIndex + 1;
            sepCount++;
        }
    }
    return (sepCount == SEP_COUNT && readMovesData(saveData, saveDataStartIndex)); // Moves
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
    int cardData = static_cast<int>(card->suit) * MAX_VALUE + card->value - 1;
    if (card->isFaceUp)
    {
        saveData[*saveDataIndex] = cardData;
    }
    else
    {
        saveData[*saveDataIndex] = cardData + 128; // Set the sign bit to 1
    }
    *saveDataIndex += 1;
}

bool Persistence::readStackData(char* saveData, int saveDataStartIndex, int saveDataEndIndex, int stackIndex)
{
    int stackLength = saveDataEndIndex - saveDataStartIndex; // endIndex is exclusive
    for (int i = 0; i < stackLength; i++)
    {
        Card* card = readCardData(saveData[saveDataStartIndex + i]);
        bool isFaceUp = card->isFaceUp;
        if (card == nullptr)
        {
            return false;
        }
        this->board->addCardToStack(stackIndex, card);
        card->isFaceUp = isFaceUp; // Since board addCardToStack sets it to true
    }
    return true;
}

bool Persistence::readFoundationData(char* saveData, int saveDataStartIndex, int saveDataEndIndex)
{
    int foundationCount = saveDataEndIndex - saveDataStartIndex; // endIndex is exclusive
    for (int i = 0; i < foundationCount; i++)
    {
        Card* card = readCardData(saveData[saveDataStartIndex + i]);
        if (card == nullptr)
        {
            return false;
        }

        int cardValue = card->value;
        char initSuitData = static_cast<char>(card->suit) * MAX_VALUE;
        for (int j = 0; j < cardValue; j++)
        {
            Card* newCard = this->deck[initSuitData + j];
            this->board->addCardToFoundation(card->suit, newCard);
        }
    }
    return true;
}

bool Persistence::readUnusedData(char* saveData, int saveDataStartIndex, int saveDataEndIndex)
{
    int unusedCardsRemaining = saveData[saveDataStartIndex]; // 1byte
    saveDataStartIndex += 1;
    int unusedCardCount = saveDataEndIndex - saveDataStartIndex;

    if (unusedCardsRemaining == 0 && unusedCardCount == 0)
    {
        return true;
    }

    Card* unusedCards[unusedCardCount];
    // 0 + 23 = 24 - 1 -> remainingCards + currUnusedIndex = unusedCardCount - 1
    int currUnusedIndex = unusedCardCount - unusedCardsRemaining - 1;

    /* Init buffer formula
    unusedCardsRemaining_unusedIndex: initBuffer
    24_-1: 24->0
    23_0: 24->0
    22_1: 23
    16_7: 17
    1_22: 2
    0_23: 1

    At remaining = totalCount, currCard is nullptr, and is not drawn, hence no offset by 1
    */

    // Skip to the topmost card in the unused pile
    int initBuffer = ((unusedCardsRemaining == unusedCardCount ? 0 : 1) + unusedCardsRemaining) % unusedCardCount;
    // Range of currBuffer: [0, unusedCardCount - 1]
    int currBuffer = initBuffer;
    int cardIndex = 0;
    do
    {
        int byteIndex = saveDataStartIndex + currBuffer;
        Card* card = readCardData(saveData[byteIndex]);
        if (card == nullptr)
        {
            return false;
        }
        unusedCards[cardIndex] = card;
        currBuffer = (currBuffer + 1) % unusedCardCount;
        cardIndex++;
    }
    while (currBuffer != initBuffer);

    // Then we add the cards to the unused Pile
    return this->board->loadUnusedCards(unusedCards, unusedCardCount, currUnusedIndex);
}

bool Persistence::readMovesData(char* saveData, int saveDataStartIndex)
{
    int moveCount = (static_cast<unsigned char>(saveData[saveDataStartIndex]) << 8) | static_cast<unsigned char>(saveData[saveDataStartIndex + 1]);
    while (this->board->getMoves() < moveCount)
    {
        this->board->addMoves();
    }
    return true;
}

Card* Persistence::readCardData(char cardData)
{
    bool isFaceUp = cardData >= 0;
    if (!isFaceUp)
    {
        cardData -= 128;
    }

    // Value checks
    if (cardData < 0 || cardData >= MAX_CARDS)
    {
        return nullptr;
    }

    // Find this card in the deck
    Card* card = this->deck[static_cast<int>(cardData)];
    if (card == nullptr)
    {
        return nullptr;
    }
    card->isFaceUp = isFaceUp;
    return card;
}