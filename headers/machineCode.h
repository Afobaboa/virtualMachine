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


//--------------------------------------------------------------------------------------------------


#endif // MACHINE_CODE_H