#ifndef VIDEO_MEMORY_H
#define VIDEO_MEMORY_H


//--------------------------------------------------------------------------------------------------


#include <stddef.h>


//--------------------------------------------------------------------------------------------------


const size_t VERTICAL_SIZE   = 7;
const size_t HORIZONTAL_SIZE = 7;

typedef char screenColor_t;
const screenColor_t COLOR_WHITE  = 'W';
const screenColor_t COLOR_YELLOW = 'Y';
const screenColor_t COLOR_GREEN  = 'G';
const screenColor_t COLOR_RED    = 'R';


//--------------------------------------------------------------------------------------------------


struct Pixel
{
    char symbol;
    screenColor_t color;
};


struct Position 
{
    size_t x;
    size_t y;
};


struct VideoMemory
{
    size_t verticalSize;
    size_t horizontalSize;
    Pixel* pixelArray;
};


//--------------------------------------------------------------------------------------------------


bool VideoMemoryInit(VideoMemory* videoMemory);
void VideoMemoryDelete(VideoMemory* videoMemory);

void VideoMemoryDraw(VideoMemory* videoMemory);
bool VideoMemoryColor(VideoMemory* videoMemory, Position* position, screenColor_t color);


//--------------------------------------------------------------------------------------------------


#endif // VIDEO_MEMORY_H