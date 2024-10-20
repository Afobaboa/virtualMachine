#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processor.h"
#include "virtualMachine.h"
#include "machineCode.h"
#include "logPrinter.h"
#include "stack.h"


//--------------------------------------------------------------------------------------------------


typedef uint64_t register64_t;
struct Registers64
{
    register64_t rax;
    register64_t rbx;
    register64_t rcx;
    register64_t rdx;
};


struct Processor
{
    MachineCode machineCode;
    Stack* stack;
    Registers64 registers;
};


//--------------------------------------------------------------------------------------------------


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

static void DoJMP(Processor* processor);
static void DoJA(Processor* processor);
static void DoJAE(Processor* processor);
static void DoJB(Processor* processor);
static void DoJBE(Processor* processor);
static void DoJE(Processor* processor);
static void DoJNE(Processor* processor);


//--------------------------------------------------------------------------------------------------


bool ExecuteProgram(const char* programName)
{
    Processor processor = {};
    ProcessorInit(&processor, programName);

    while (processor.machineCode.instructionNum < processor.machineCode.instructionCount)
    {
        // printf("%zu\n", processor.machineCode.instructionNum);
        if (!InstructionExecute(&processor))
        {
            ProcessorDelete(&processor);
            return false;
        }
    }

    ProcessorDelete(&processor);
    return true;
}


//--------------------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName)
{
    MachineCodeInitFromFile(&(processor->machineCode), (char*) programName);
    processor->registers = {};
    STACK_CREATE(processor->stack, sizeof(instruction_t));
}


static void ProcessorDelete(Processor* processor)
{
    processor->registers = {};
    MachineCodeDelete(&(processor->machineCode));
    StackDelete(&(processor->stack));
}


#define DO_CMD_IN_CASE(CMD_NAME)    \
{                                   \
    case CMD_NAME:                  \
        Do##CMD_NAME(processor);    \
        break;                      \
}


static bool InstructionExecute(Processor* processor)
{
    instruction_t cmdName = 0;
    MachineCodeGetNextInstruction(&(processor->machineCode), &cmdName);

    switch (cmdName)
    {
    DO_CMD_IN_CASE(PUSH);
    DO_CMD_IN_CASE(ADD);
    DO_CMD_IN_CASE(SUB);
    DO_CMD_IN_CASE(MUL);
    DO_CMD_IN_CASE(DIV);
    DO_CMD_IN_CASE(OUT);
    DO_CMD_IN_CASE(JMP);

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
    instruction_t elemToPush = 0;
    MachineCodeGetNextInstruction(&(processor->machineCode), &elemToPush);
    StackPush(processor->stack, &elemToPush);
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

static void DoJMP(Processor* processor)
{
    instruction_t instructionNum = 0;
    MachineCodeGetNextInstruction(&(processor->machineCode), &instructionNum);
    MachineCodeJump(&(processor->machineCode), JUMP_ABSOLUTE, (int64_t) instructionNum);
}


#define JUMP_IF(CONDITION)                                      \
{                                                               \
    instruction_t lastInstruction    = 0;                       \
    instruction_t preLastInstruction = 0;                       \
                                                                \
    if (StackPop(processor->stack, &lastInstruction) != OK ||   \
        StackPop(processor->stack, &preLastInstruction) != OK)  \
    {                                                           \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);    \
        return;                                                 \
    }                                                           \
                                                                \
    if (lastInstruction CONDITION preLastInstruction)           \
        DoJMP(processor);                                       \
    else                                                        \
        MachineCodeSkipInstruction(&processor->machineCode);    \
}


static void DoJA(Processor* processor)
{
    JUMP_IF(>);
}


static void DoJAE(Processor* processor)
{
    JUMP_IF(>=);
}


static void DoJB(Processor* processor)
{
    JUMP_IF(<)
}


static void DoJBE(Processor* processor)
{
    JUMP_IF(<=);
}


static void DoJE(Processor* processor)
{
    JUMP_IF(==);
}


static void DoJNE(Processor* processor)
{
    JUMP_IF(!=);
}
#undef JUMP_IF
