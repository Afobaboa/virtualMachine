#ifndef RAM_H
#define RAM_H


//--------------------------------------------------------------------------------------------------


#include <stddef.h>
#include <stdint.h>


//--------------------------------------------------------------------------------------------------

const size_t RAM_CAPACITY = 512;


typedef uint64_t memoryCell_t;
struct RAM 
{
    memoryCell_t* memory;
};


//--------------------------------------------------------------------------------------------------


bool RamInit(RAM* ram);
void RamDelete(RAM* ram);

bool RamCellSet(RAM* ram, size_t cellNum, memoryCell_t value);
void RamScreenDraw(RAM* ram);


//--------------------------------------------------------------------------------------------------


#endif // RAM_H