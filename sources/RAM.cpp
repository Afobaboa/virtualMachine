#include <stdlib.h>

#include "RAM.h"
#include "videoMemory.h"
#include "logPrinter.h"


//--------------------------------------------------------------------------------------------------



bool RamInit(RAM* ram)
{
    if (sizeof(Pixel) * VERTICAL_SIZE * HORIZONTAL_SIZE > RAM_CAPACITY * sizeof(memoryCell_t))
    {
        LOG_PRINT(ERROR, "RAM don't have enough memory to contain video memory.\n");
        return false;
    }
    
    ram->memory = (memoryCell_t*) calloc(RAM_CAPACITY, sizeof(memoryCell_t));
    if (ram->memory == NULL)
        return false;

    VideoMemoryReset((VideoMemory*) &ram->memory);

    return true;
}


void RamDelete(RAM* ram)
{
    free(ram->memory);
    ram->memory = NULL;
}


bool RamGetValue(RAM* ram, size_t cellNum, memoryCell_t* valueBuffer)
{
    if (cellNum >= RAM_CAPACITY)
        return false;

    *valueBuffer = ram->memory[cellNum];
    return true;
}


bool RamCellSet(RAM* ram, size_t cellNum, memoryCell_t value)
{
    if (cellNum > RAM_CAPACITY)
        return false;

    ram->memory[cellNum] = value;
    return true;
}


void RamScreenDraw(RAM* ram)
{
    // ColoredPrintf(GREEN, "ram->memory = %p\n", ram->memory);
    VideoMemoryDraw((VideoMemory*) &ram->memory);
}
