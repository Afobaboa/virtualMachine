#include <stdlib.h>

#include "machineCode.h"


//--------------------------------------------------------------------------------------------------


bool MachineCodeInit(MachineCode* machineCode)
{

}


bool MachineCodeInitFromFile(MachineCode* machineCode, char* fileName)
{

}


void MachineCodeDelete(MachineCode* machineCode)
{

}


codeStatus_t MachineCodeGetNextInstruction(MachineCode* machineCode, 
                                           instruction_t* instructionBuffer)
{

}


codeStatus_t MachineCodeAddInstruction(MachineCode* machineCode, const instruction_t instruction)
{

}


codeStatus_t MachineCodeJump(MachineCode* machineCode, const size_t instructionNum)
{

}