#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processor.h"
#include "stack.h"


//----------------------------------------------------------------------------------------


typedef uint64_t register64_t;
struct Registers64
{
    register64_t rax;
    register64_t rbx;
    register64_t rcx;
    register64_t rdx;
};


typedef uint32_t register32_t;
struct Registers32
{
    register32_t eax;
    register32_t ebx;
    register32_t ecx;
    register32_t edx;
};


struct Registers
{
    Registers64 register64;
    Registers32 register32;
};


struct Processor
{
    MachineCode machineCode;
    Stack* stack;
    Registers registers;
};


//----------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName);


static void ProcessorDelete(Processor* processor);


static bool InstructionExecute(Processor* processor);



static void DoPUSH(Processor* processor);
static void DoADD(Processor* processor);
static void DoSUB(Processor* processor);
static void DoDIV(Processor* processor);
static void DoMUL(Processor* processor);
static bool DoIN(Processor* processor);
static void DoOUT(Processor* processor);


//----------------------------------------------------------------------------------------


bool ExecuteProgram(const char* programName)
{
    Processor processor = {};
    ProcessorInit(&processor, programName);

    while (processor.machineCode.instructionNum < processor.machineCode.instructionCount)
        if (!InstructionExecute(&processor))
            return false;

    ProcessorDelete(&processor);
    return true;
}


//----------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName)
{
    FILE* executableFile = fopen(programName, "rb");
    fread(&(processor->machineCode.instructionCount), 
          sizeof(instruction_t), 1, executableFile);
    processor->machineCode.instructionNum = 0;

    const size_t instructionCount = processor->machineCode.instructionCount;
    processor->machineCode.code = (instruction_t*) calloc(instructionCount,
                                                          sizeof(instruction_t));
    for (size_t instructionNum = 0; instructionNum < instructionCount; instructionNum++)
    {
        fread(processor->machineCode.code + instructionNum,
              sizeof(instruction_t), 1, executableFile);
    }
    fclose(executableFile);

    STACK_CREATE(processor->stack, sizeof(instruction_t));
}


static void ProcessorDelete(Processor* processor)
{
    free(processor->machineCode.code);

    processor->machineCode.instructionCount = 0;
    processor->machineCode.instructionNum   = 0;
    processor->machineCode.code             = NULL;
    processor->registers                    = {};

    StackDelete(&(processor->stack));
}


#define DO_CMD_IN_CASE(CMD_NAME) \
{   \
    case CMD_NAME: \
        Do##CMD_NAME(processor); \
        break; \
}


static bool InstructionExecute(Processor* processor)
{
    size_t instructionNum =processor->machineCode.instructionNum;
    cmdName_t cmdName = (cmdName_t) processor->machineCode.code[instructionNum];
    processor->machineCode.instructionNum++;

    switch (cmdName)
    {
    DO_CMD_IN_CASE(PUSH);
    DO_CMD_IN_CASE(ADD);
    DO_CMD_IN_CASE(SUB);
    DO_CMD_IN_CASE(MUL);
    DO_CMD_IN_CASE(DIV);
    DO_CMD_IN_CASE(OUT);

    case IN:
        if (!DoIN(processor))
            return false;
        break;

    case WRONG:
    default:
        LOG_PRINT(ERROR, "Wrong cmdName = %d\n", cmdName);
        return false;
    }

    return true;
}
#undef DO_CMD_IN_CASE


static void DoPUSH(Processor* processor)
{
    size_t instructionNum = processor->machineCode.instructionNum;
    instruction_t pushedElem = processor->machineCode.code[instructionNum];
    processor->machineCode.instructionNum++;
    StackPush(processor->stack, &pushedElem);
}


#define DO_OPERATION(operation)                                         \
{                                                                       \
    instruction_t firstPoppedElem  = 0;                                 \
    instruction_t secondPoppedElem = 0;                                 \
    if ((StackPop(processor->stack, &firstPoppedElem)  != OK) ||        \
        (StackPop(processor->stack, &secondPoppedElem) != OK))          \
    {                                                                   \
        LOG_PRINT(INFO, "Stack ptr = %p\n", processor->stack);          \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);            \
        return;                                                         \
    }                                                                   \
                                                                        \
    instruction_t result = firstPoppedElem operation secondPoppedElem;  \
    StackPush(processor->stack, &result);                               \
}


static void DoADD(Processor* processor)
{
    DO_OPERATION(+);
}


static void DoSUB(Processor* processor)
{
    DO_OPERATION(-);
}


static void DoDIV(Processor* processor)
{
    DO_OPERATION(/);
}


static void DoMUL(Processor* processor)
{
    DO_OPERATION(*);
}
#undef DO_OPERATION


static bool DoIN(Processor* processor)
{
    instruction_t inputNum = 0;
    if (scanf("%zu", &inputNum) <= 0)
        return false;

    StackPush(processor->stack, &inputNum);
    return true;
}


static void DoOUT(Processor* processor)
{
    instruction_t lastElem = 0;
    if (StackPop(processor->stack, &lastElem) != OK)
    {
        ColoredPrintf(RED, "OUT: POP ERROR\n");
        return;
    }

    ColoredPrintf(YELLOW, "%d\n", lastElem);
}
