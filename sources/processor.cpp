#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "processor.h"
#include "virtualMachine.h"
#include "machineCode.h"
#include "logPrinter.h"
#include "stack.h"
#include "RAM.h"


//--------------------------------------------------------------------------------------------------


struct Processor
{
    MachineCode machineCode;
    Stack* stack;
    Stack* callStack;
    Registers64 registers;
    RAM ram;
};


//--------------------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName);


static void ProcessorDelete(Processor* processor);


static bool InstructionExecute(Processor* processor);


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
    STACK_CREATE(processor->stack,     sizeof(instruction_t));
    STACK_CREATE(processor->callStack, sizeof(instruction_t));
    RamInit(&processor->ram);
}


static void ProcessorDelete(Processor* processor)
{
    processor->registers = {};
    MachineCodeDelete(&(processor->machineCode));
    StackDelete(&processor->stack);
    StackDelete(&processor->callStack);
    RamDelete(&processor->ram);
}


#define DEF_CMD_(CMD_NAME, CMD_SET, DO_CMD) \
{                                           \
    case CMD_NAME:                          \
        DO_CMD;                             \
        break;                              \
}

static bool InstructionExecute(Processor* processor)
{
    instruction_t cmdName = 0;
    MachineCodeGetNextInstruction(&(processor->machineCode), &cmdName);

    switch (cmdName)
    {
    #include "commands.h"

    case CMD_NAME_WRONG:
    default:
        LOG_PRINT(ERROR, "Wrong cmdName = %d\n", cmdName);
        return false;
    }

    return true;
}
#undef DEF_CMD_
