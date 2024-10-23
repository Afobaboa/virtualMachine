#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processor.h"
#include "virtualMachine.h"
#include "machineCode.h"
#include "logPrinter.h"
#include "stack.h"
#include "RAM.h"


//--------------------------------------------------------------------------------------------------


typedef uint64_t register64_t;
struct Registers64
{
    register64_t RAX;
    register64_t RBX;
    register64_t RCX;
    register64_t RDX;
};


struct Processor
{
    MachineCode machineCode;
    Stack* stack;
    Registers64 registers;
    RAM ram;
};


//--------------------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName);


static void ProcessorDelete(Processor* processor);


static bool InstructionExecute(Processor* processor);


static void DoPUSH(Processor* processor);
static void DoPOP(Processor* processor);

static void DoADD(Processor* processor);
static void DoSUB(Processor* processor);
static void DoDIV(Processor* processor);
static void DoMUL(Processor* processor);

static bool DoIN(Processor* processor);
static void DoOUT(Processor* processor);

static void DoDRAW(Processor* processor);

static void DoJMP(Processor* processor);
static void DoJA(Processor* processor);
static void DoJAE(Processor* processor);
static void DoJB(Processor* processor);
static void DoJBE(Processor* processor);
static void DoJE(Processor* processor);
static void DoJNE(Processor* processor);


static instruction_t RegisterGetValue(Processor* processor, registerName_t registerName);


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
    RamInit(&processor->ram);
}


static void ProcessorDelete(Processor* processor)
{
    processor->registers = {};
    MachineCodeDelete(&(processor->machineCode));
    StackDelete(&(processor->stack));
    RamDelete(&processor->ram);
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
    DO_CMD_IN_CASE(POP);

    DO_CMD_IN_CASE(ADD);
    DO_CMD_IN_CASE(SUB);
    DO_CMD_IN_CASE(MUL);
    DO_CMD_IN_CASE(DIV);

    DO_CMD_IN_CASE(OUT);
    DO_CMD_IN_CASE(DRAW);

    DO_CMD_IN_CASE(JMP);
    DO_CMD_IN_CASE(JA);
    DO_CMD_IN_CASE(JAE);
    DO_CMD_IN_CASE(JB);
    DO_CMD_IN_CASE(JBE);
    DO_CMD_IN_CASE(JE);
    DO_CMD_IN_CASE(JNE);

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
    PushPopMode pushMode = {};
    MachineCodeGetNextInstruction(&processor->machineCode, (instruction_t*) &pushMode);
    
    instruction_t result = 0;
    if (pushMode.isRegister)
    {
        instruction_t registerName = 0;
        MachineCodeGetNextInstruction(&processor->machineCode, &registerName);
        result += RegisterGetValue(processor, (registerName_t) registerName);
    }
    
    if (pushMode.isConst)
    {
        instruction_t instructionBuffer = 0;
        MachineCodeGetNextInstruction(&processor->machineCode, &instructionBuffer);
        result += instructionBuffer;
    }

    if (pushMode.isRAM)
    {
        instruction_t value;
        RamGetValue(&processor->ram, result, &value);
        StackPush(processor->stack, &value);
    }
    else 
    {
        StackPush(processor->stack, &result);
    }
}


static void DoPOP(Processor* processor)
{
    PushPopMode popMode = {};
    MachineCodeGetNextInstruction(&processor->machineCode, (instruction_t*) &popMode);
    
    instruction_t nextInstruction = 0;
    if (popMode.isRAM)
    {
        size_t cellNum = 0;
        if (popMode.isRAM)
        {
            MachineCodeGetNextInstruction(&processor->machineCode, &nextInstruction);
            cellNum += RegisterGetValue(processor, (registerName_t) nextInstruction);
        }

        if (popMode.isConst)
        {
            MachineCodeGetNextInstruction(&processor->machineCode, &nextInstruction);
            cellNum += nextInstruction;
        }

        instruction_t value = 0;
        if (!StackPop(processor->stack, &value))
            ColoredPrintf(RED, "CAN'T POP!!!\n");  

        RamCellSet(&processor->ram, cellNum, value);
    }

    else 
    {
        if (popMode.isRegister)
        {
            instruction_t registerName = 0;
            MachineCodeGetNextInstruction(&processor->machineCode, &registerName);

            instruction_t value = 0;
            StackPop(processor->stack, &value);
            
            *((register64_t*) &processor->registers + registerName - RAX) = value;
        }
    }
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


static void DoADD(Processor* processor) { DO_OPERATION(+); }
static void DoSUB(Processor* processor) { DO_OPERATION(-); }
static void DoDIV(Processor* processor) { DO_OPERATION(/); }
static void DoMUL(Processor* processor) { DO_OPERATION(*); }
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


static void DoDRAW(Processor* processor)
{
    RamScreenDraw(&processor->ram);
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
    {                                                           \
        StackPush(processor->stack, &preLastInstruction);       \
        StackPush(processor->stack, &lastInstruction);          \
        DoJMP(processor);                                       \
    }                                                           \
    else                                                        \
        MachineCodeSkipInstruction(&processor->machineCode);    \
}

static void DoJA(Processor* processor)  { JUMP_IF(>);  }
static void DoJAE(Processor* processor) { JUMP_IF(>=); }
static void DoJB(Processor* processor)  { JUMP_IF(<);  }
static void DoJBE(Processor* processor) { JUMP_IF(<=); }
static void DoJE(Processor* processor)  { JUMP_IF(==); }
static void DoJNE(Processor* processor) { JUMP_IF(!=); }
#undef JUMP_IF


#define REGISTER_GET_VALUE_CASE(REGISTER_NAME) \
{\
    if (registerName == REGISTER_NAME)\
        return processor->registers.REGISTER_NAME;\
}


static instruction_t RegisterGetValue(Processor* processor, registerName_t registerName)
{
    REGISTER_GET_VALUE_CASE(RAX);
    REGISTER_GET_VALUE_CASE(RBX);
    REGISTER_GET_VALUE_CASE(RCX);
    REGISTER_GET_VALUE_CASE(RDX);

    ColoredPrintf(RED, "WRONG REGISTER NAME\n");
    return 0;
}
#undef REGISTER_GET_VALUE_CASE


#define CONVERT_TO_REGISTER_CASE(REGISTER_NAME)         \
{                                                       \
    if (strcmp(string, GET_NAME(REGISTER_NAME)) == 0)   \
        return REGISTER_NAME;                           \
}
