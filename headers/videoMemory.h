#ifndef VIDEO_MEMORY_H
#define VIDEO_MEMORY_H


//--------------------------------------------------------------------------------------------------


#include <stddef.h>
#include <stdint.h>


//--------------------------------------------------------------------------------------------------


#define GET_POSITION(x, y) \
    (Position) {x, y}


const size_t VERTICAL_SIZE   = 7;
const size_t HORIZONTAL_SIZE = VERTICAL_SIZE * 2;

typedef uint64_t screenColor_t;
const screenColor_t PIXEL_COLOR_WHITE  = 'W';
const screenColor_t PIXEL_COLOR_YELLOW = 'Y';
const screenColor_t PIXEL_COLOR_GREEN  = 'G';
const screenColor_t PIXEL_COLOR_RED    = 'R';


//--------------------------------------------------------------------------------------------------


typedef uint64_t symbol_t;

struct Pixel
{
    symbol_t symbol;
    screenColor_t color;
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
bool VideoMemorySetColor(VideoMemory* videoMemory, Position position, screenColor_t color);
bool VideoMemorySetSymbol(VideoMemory* videoMemory, Position position, char symbol);


//--------------------------------------------------------------------------------------------------


#endif // VIDEO_MEMORY_H