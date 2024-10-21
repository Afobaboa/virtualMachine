#ifndef VIDEO_MEMORY_H
#define VIDEO_MEMORY_H


//--------------------------------------------------------------------------------------------------


#include "RAM.h"


//--------------------------------------------------------------------------------------------------


#define GET_POSITION(x, y) \
    (Position) {x, y}


const size_t VERTICAL_SIZE   = 7;
const size_t HORIZONTAL_SIZE = VERTICAL_SIZE * 2;

typedef memoryCell_t pixelColor_t;
const pixelColor_t PIXEL_COLOR_WHITE  = 'W';
const pixelColor_t PIXEL_COLOR_YELLOW = 'Y';
const pixelColor_t PIXEL_COLOR_GREEN  = 'G';
const pixelColor_t PIXEL_COLOR_RED    = 'R';


//--------------------------------------------------------------------------------------------------


typedef memoryCell_t pixelSymbol_t;

struct Pixel
{
    pixelSymbol_t symbol;
    pixelColor_t color;
};


struct Position 
{
    size_t x;
    size_t y;
};


struct VideoMemory
{
    Pixel* pixelArray;
};


//--------------------------------------------------------------------------------------------------


bool VideoMemoryInit(VideoMemory* videoMemory);
void VideoMemoryDelete(VideoMemory* videoMemory);

void VideoMemoryDraw(VideoMemory* videoMemory);
bool VideoMemorySetColor(VideoMemory* videoMemory, Position position, pixelColor_t color);
bool VideoMemorySetSymbol(VideoMemory* videoMemory, Position position, char symbol);
void VideoMemoryReset(VideoMemory* videoMemory);


//--------------------------------------------------------------------------------------------------


#endif // VIDEO_MEMORY_H