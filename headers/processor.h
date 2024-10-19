#ifndef PROCESSOR_H
#define PROCESSOR_H


//----------------------------------------------------------------------------------------


#include <stdint.h>
#include <stddef.h>


 //---------------------------------------------------------------------------------------


typedef uint64_t instruction_t;

 struct MachineCode
 {
    size_t instructionCount;
    size_t instructionNum;
    instruction_t* code;
 };



//----------------------------------------------------------------------------------------


bool ExecuteProgram(const char* programName);


//----------------------------------------------------------------------------------------


#endif // PROCESSOR_H