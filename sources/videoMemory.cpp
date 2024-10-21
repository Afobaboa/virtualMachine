#include <stdio.h>
#include <stdlib.h>

#include "videoMemory.h"
#include "logPrinter.h"


//--------------------------------------------------------------------------------------------------


static bool VideoMemoryIsIn(Position* position);
static bool IsColor(screenColor_t color);

static void PixelPrint(Pixel* pixel);


//--------------------------------------------------------------------------------------------------


bool VideoMemoryInit(VideoMemory* videoMemory)
{

}


void VideoMemoryDelete(VideoMemory* videoMemory)
{

}


void VideoMemoryDraw(VideoMemory* videoMemory)
{
    Pixel* pixel = NULL;
    for (size_t y = 0; y < VERTICAL_SIZE; y++)
    {
        for (size_t x = 0; x < HORIZONTAL_SIZE; x++)
        {
            pixel = videoMemory->pixelArray + y * HORIZONTAL_SIZE + x;
            PixelPrint(pixel);
        }
        putchar('\n');
    }
}


bool VideoMemoryColor(VideoMemory* videoMemory, Position* position, screenColor_t color)
{
    if (!IsColor(color) || !VideoMemoryIsIn(position))
        return false;

    Pixel* pixel = videoMemory->pixelArray + position->y * HORIZONTAL_SIZE + position->x;
    pixel->color = color;

    return true;
}


//--------------------------------------------------------------------------------------------------


static bool VideoMemoryIsIn(Position* position)
{
    if (position->x < HORIZONTAL_SIZE && position->y < VERTICAL_SIZE)
        return true;
    return false;
}


static bool IsColor(screenColor_t color)
{
    if (color == COLOR_GREEN ||
        color == COLOR_RED ||
        color == COLOR_WHITE ||
        color == COLOR_YELLOW)
    {
        return true;
    }
    return false;
}


static void PixelPrint(Pixel* pixel)
{

}
