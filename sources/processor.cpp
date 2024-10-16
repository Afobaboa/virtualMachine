#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/processor.h"
#include "../stack/headers/stack.h"


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

typedef int instruction_t;
struct Processor
{
    size_t instructionCount;
    size_t instructionNum;
    instruction_t* machineCode;

    Stack* stack;
    Registers64 registers64;
    Registers32 registers32;
};


//----------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName);


static void ProcessorDelete(Processor* processor);


static void InstructionExecute(Processor* processor);



static void DoPush(Processor* processor);
static void DoAdd(Processor* processor);
static void DoSub(Processor* processor);
static void DoDiv(Processor* processor);
static void DoMul(Processor* processor);
static void DoIn(Processor* processor);
static void DoOut(Processor* processor);


//----------------------------------------------------------------------------------------


void ExecuteProgram(const char* programName)
{
    Processor processor = {};
    ProcessorInit(&processor, programName);

    while (processor.instructionNum < processor.instructionCount)
        InstructionExecute(&processor);

    ProcessorDelete(&processor); 
}


//----------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName)
{
    FILE* executableFile = fopen(programName, "r");
    fscanf(executableFile, "%zu", &(processor->instructionCount));
    processor->instructionNum = 0;

    processor->machineCode = (instruction_t*) calloc(processor->instructionCount, 
                                                     sizeof(instruction_t));
    for (size_t instructionNum = 0; instructionNum < processor->instructionCount; 
                                                                instructionNum++)
    {
        fscanf(executableFile, "%d", processor->machineCode + instructionNum);
    }
    fclose(executableFile);

    STACK_CREATE(processor->stack, sizeof(instruction_t));
}


static void ProcessorDelete(Processor* processor)
{
    free(processor->machineCode);

    processor->instructionCount = 0;
    processor->instructionNum   = 0;
    processor->machineCode      = NULL;
    processor->registers32      = {};
    processor->registers64      = {};

    StackDelete(&(processor->stack));
}


static void InstructionExecute(Processor* processor)
{
    cmdName_t cmdName = (cmdName_t) processor->machineCode[processor->instructionNum];
    processor->instructionNum++;

    switch (cmdName)
    {
    case PUSH:
        DoPush(processor);
        break;

    case ADD:
        DoAdd(processor);
        break;
    
    case SUB:
        DoSub(processor);
        break;

    case DIV:
        DoDiv(processor);
        break;

    case MUL:
        DoMul(processor);
        break;

    case IN:
        DoIn(processor);
        break;

    case OUT:
        DoOut(processor);
        break;

    case WRONG:
    default:
        LOG_PRINT(ERROR, "Wrong cmdName = %d\n", cmdName);
    }
}


static void DoPush(Processor* processor)
{
    int pushedElem = processor->machineCode[processor->instructionNum];
    processor->instructionNum++;
    StackPush(processor->stack, &pushedElem);
}


#define DO_OPERATION(operation)                                         \
{                                                                       \
    int firstPoppedElem  = 0;                                           \
    int secondPoppedElem = 0;                                           \
    if ((StackPop(processor->stack, &firstPoppedElem)  != OK) ||        \
        (StackPop(processor->stack, &secondPoppedElem) != OK))          \
    {                                                                   \
        LOG_PRINT(INFO, "Stack ptr = %p\n", processor->stack);          \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);            \
        return;                                                         \
    }                                                                   \
                                                                        \
    int result = firstPoppedElem operation secondPoppedElem;            \
    StackPush(processor->stack, &result);                               \
}


static void DoAdd(Processor* processor)
{
    DO_OPERATION(+);
}


static void DoSub(Processor* processor)
{
    DO_OPERATION(-);
}


static void DoDiv(Processor* processor)
{
    DO_OPERATION(/);
}


static void DoMul(Processor* processor)
{
    DO_OPERATION(*);
}
#undef DO_OPERATION

static void DoIn(Processor* processor)
{
    ColoredPrintf(YELLOW, "IN\n");
}


static void DoOut(Processor* processor)
{
    int lastElem = 0;
    if (StackPop(processor->stack, &lastElem) != OK)
    {
        ColoredPrintf(RED, "OUT: POP ERROR\n");
        return;
    }

    ColoredPrintf(YELLOW, "%d\n", lastElem);
}
