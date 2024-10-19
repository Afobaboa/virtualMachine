#ifndef MACHINE_CODE_H
#define MACHINE_CODE_H


//--------------------------------------------------------------------------------------------------


#include <stddef.h>
#include <stdint.h>


//--------------------------------------------------------------------------------------------------


typedef uint64_t instruction_t;

 struct MachineCode
 {
    size_t instructionCount;
    size_t instructionNum;
    instruction_t* code;
 };


 //-------------------------------------------------------------------------------------------------


 enum CODE_STATUSES
{
    CODE_OK,
    CODE_LEFT,
    CODE_OVERFLOW
};
typedef enum CODE_STATUSES codeStatus_t;


//--------------------------------------------------------------------------------------------------


const size_t FIRST_INSTRUCTION_NUM = 0;

const char* const MACHINE_CODE_FILE_EXTENSION = ".vm";


//--------------------------------------------------------------------------------------------------


bool MachineCodeInit(MachineCode* machineCode);


bool MachineCodeInitFromFile(MachineCode* machineCode, char* fileName);


void MachineCodeDelete(MachineCode* machineCode);


codeStatus_t MachineCodeGetNextInstruction(MachineCode* machineCode, 
                                           instruction_t* instructionBuffer);


codeStatus_t MachineCodeAddInstruction(MachineCode* machineCode, const instruction_t instruction);


codeStatus_t MachineCodeJump(MachineCode* machineCode, const size_t instructionNum);


bool MachineCodeWriteToFile(MachineCode* machineCode, char* fileName);


//--------------------------------------------------------------------------------------------------


#endif // MACHINE_CODE_H