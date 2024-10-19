#include <stdio.h>
#include <stdlib.h>

#include "machineCode.h"
#include "logPrinter.h"


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


bool MachineCodeWriteToFile(MachineCode* machineCode, char* fileName)
{
    FILE* file = fopen(fileName, "wb");
    if (file == NULL)
    {
        ColoredPrintf(RED, "Can't write machine code to %s.\n", fileName);
        return false;
    }

    fwrite(&(machineCode->instructionCount), sizeof(size_t), 1, file);
    fwrite(machineCode->code, sizeof(instruction_t), machineCode->instructionCount, file);

    fclose(file);
    return true;
}
