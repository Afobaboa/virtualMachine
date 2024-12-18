#ifndef MACHINE_CODE_H
#define MACHINE_CODE_H


//--------------------------------------------------------------------------------------------------


#include <stddef.h>
#include <stdint.h>


//--------------------------------------------------------------------------------------------------


typedef int64_t instruction_t;

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
    CODE_OVERFLOW,
    CODE_UNDERFLOW
};
typedef enum CODE_STATUSES codeStatus_t;


enum JUMP_MODES
{
    JUMP_ABSOLUTE,
    JUMP_RELATIVE
};
typedef enum JUMP_MODES jumpMode_t;


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


codeStatus_t MachineCodeJump(MachineCode* machineCode, const jumpMode_t jumpMode,
                                                       const int64_t    instructionShift);


bool MachineCodeWriteToFile(MachineCode* machineCode, char* fileName);


size_t MachineCodeGetInstructionNum(MachineCode* machineCode);


void MachineCodeSkipInstruction(MachineCode* machineCode);


//--------------------------------------------------------------------------------------------------


#endif // MACHINE_CODE_H