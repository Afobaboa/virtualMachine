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
    videoMemory->pixelArray = (Pixel*) calloc(HORIZONTAL_SIZE * VERTICAL_SIZE, sizeof(Pixel));
    if (videoMemory->pixelArray == NULL)
        return false;

    Pixel* pixel = NULL;
    for (size_t pixelNum = 0; pixelNum < HORIZONTAL_SIZE * VERTICAL_SIZE; pixelNum++)
    {
        pixel = videoMemory->pixelArray + pixelNum;
        *pixel = {.symbol = ' ',
                  .color  = PIXEL_COLOR_WHITE};
    }

    return true;
}


void VideoMemoryDelete(VideoMemory* videoMemory)
{
    free(videoMemory->pixelArray);
    videoMemory->pixelArray = NULL;
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


bool VideoMemorySetColor(VideoMemory* videoMemory, Position position, screenColor_t color)
{
    if (!IsColor(color) || !VideoMemoryIsIn(&position))
        return false;

    Pixel* pixel = videoMemory->pixelArray + position.y * HORIZONTAL_SIZE + position.x;
    pixel->color = color;

    return true;
}


bool VideoMemorySetSymbol(VideoMemory* videoMemory, Position position, char symbol)
{
    if (!VideoMemoryIsIn(&position))
        return false;

    Pixel* pixel = videoMemory->pixelArray + position.y * HORIZONTAL_SIZE + position.x;
    pixel->symbol = symbol;
    
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
    if (color == PIXEL_COLOR_GREEN ||
        color == PIXEL_COLOR_RED   ||
        color == PIXEL_COLOR_WHITE ||
        color == PIXEL_COLOR_YELLOW)
    {
        return true;
    }

    return false;
}


#define PIXEL_PRINT_IN_CASE(COLOR_NAME)                 \
{                                                       \
    if (pixel->color == PIXEL_COLOR_##COLOR_NAME)              \
    {                                                   \
        ColoredPrintf(COLOR_NAME, "%c", pixel->symbol);  \
        return;                                         \
    }                                                   \
}


static void PixelPrint(Pixel* pixel)
{
    PIXEL_PRINT_IN_CASE(WHITE);
    PIXEL_PRINT_IN_CASE(YELLOW);
    PIXEL_PRINT_IN_CASE(GREEN);
    PIXEL_PRINT_IN_CASE(RED);

}
#undef PIXEL_PRINT_IN_CASE
