#include <stdio.h>
#include <stdlib.h>

#include "fileProcessor.h"
#include "machineCode.h"
#include "logPrinter.h"


//--------------------------------------------------------------------------------------------------


static const size_t maxInstructionsCount = 1024;


//--------------------------------------------------------------------------------------------------


bool MachineCodeInit(MachineCode* machineCode)
{
    machineCode->instructionCount = maxInstructionsCount;
    machineCode->instructionNum   = FIRST_INSTRUCTION_NUM;

    machineCode->code = (instruction_t*) calloc(maxInstructionsCount + 1, sizeof(instruction_t));
    if (machineCode == NULL)
        return false;

    return true;
}


bool MachineCodeInitFromFile(MachineCode* machineCode, char* fileName)
{
    if (!FileNameCheckExtension(fileName, MACHINE_CODE_FILE_EXTENSION))
        return false;

    FILE* machineCodeFile = fopen(fileName, "rb");
    if (machineCodeFile == NULL)
        return false;

    fread(&(machineCode->instructionCount), sizeof(instruction_t), 1, machineCodeFile);
    machineCode->code = (instruction_t*) calloc(machineCode->instructionCount, 
                                                sizeof(instruction_t));
    if (machineCode->code == NULL)
    {
        fclose(machineCodeFile);
        return false;
    }

    fread(machineCode->code, sizeof(instruction_t), machineCode->instructionCount, machineCodeFile);
    machineCode->instructionNum = 0;

    fclose(machineCodeFile);
    return true;
}


void MachineCodeDelete(MachineCode* machineCode)
{
    free(machineCode->code);
    machineCode->code = NULL;

    machineCode->instructionCount = 0;
    machineCode->instructionNum   = 0;
}


codeStatus_t MachineCodeGetNextInstruction(MachineCode* machineCode, 
                                           instruction_t* instructionBuffer)
{
    if (machineCode->instructionNum == machineCode->instructionCount)
        return CODE_LEFT;

    *instructionBuffer = machineCode->code[machineCode->instructionNum];
    (machineCode->instructionNum)++;
    return CODE_OK;
}


codeStatus_t MachineCodeAddInstruction(MachineCode* machineCode, const instruction_t instruction)
{
    if (machineCode->instructionNum >= machineCode->instructionCount)
        return CODE_OVERFLOW;
    
    machineCode->code[machineCode->instructionNum] = instruction;
    (machineCode->instructionNum)++;
    return CODE_OK;
}


codeStatus_t MachineCodeJump(MachineCode* machineCode, const size_t instructionNum)
{
    if (instructionNum >= machineCode->instructionCount)
        return CODE_OVERFLOW;
    
    machineCode->instructionNum = instructionNum;
    return CODE_OK;
}


bool MachineCodeWriteToFile(MachineCode* machineCode, char* fileName)
{
    FILE* file = fopen(fileName, "wb");
    if (file == NULL)
    {
        ColoredPrintf(RED, "Can't write machine code to %s.\n", fileName);
        return false;
    }

    fwrite(&(machineCode->instructionNum), sizeof(size_t), 1, file);
    fwrite(machineCode->code, sizeof(instruction_t), machineCode->instructionNum, file);

    fclose(file);
    return true;
}
