#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/processor.h"
#include "../stack/headers/stack.h"


//----------------------------------------------------------------------------------------


#define DO_OPERATION(operation)                                 \
    int firstPoppedElem  = 0;                                   \
    int secondPoppedElem = 0;                                   \
    if ((StackPop(stack, &firstPoppedElem)  != OK) ||           \
        (StackPop(stack, &secondPoppedElem) != OK))             \
    {                                                           \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);    \
        return;                                                 \
    }                                                           \
                                                                \
    int result = firstPoppedElem operation secondPoppedElem;    \
    StackPush(stack, &result);                              


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


static Stack* stack = NULL;


//----------------------------------------------------------------------------------------


static void ProcessorInit(Processor* processor, const char* programName);


static void ProcessorDelete(Processor* processor);


static void InstructionExecute(Processor* processor);



static void DoStart();
static void DoPush();
static void DoAdd();
static void DoSub();
static void DoDiv();
static void DoMul();
static void DoIn();
static void DoOut();
static void DoEnd(); 


//----------------------------------------------------------------------------------------


void ExecuteProgram(const char* programName)
{
    Processor processor = {};
    ProcessorInit(&processor, programName);

    while (processor.instructionNum < processor.instructionCount)
        InstructionExecute(&processor);

    ProcessorDelete(&processor); 
}


cmdName_t GetCommand()
{
    const size_t maxCommandLength = 10;
    static char command[maxCommandLength + 1] = "";
    char commandScanfFormat[10] = "";
    sprintf(commandScanfFormat, "%%%zus", maxCommandLength);
    scanf(commandScanfFormat, command);

    if (command[maxCommandLength - 1] != '\0')
    {
        ColoredPrintf(RED, "SYNTAX ERROR\n");
        return WRONG;
    }

    if (strcmp(command, "START") == 0)
        return START;

    if (strcmp(command, "PUSH") == 0)
        return PUSH;
    
    if (strcmp(command, "ADD") == 0)
        return ADD;

    if (strcmp(command, "SUB") == 0)
        return SUB;

    if (strcmp(command, "DIV") == 0)
        return DIV;

    if (strcmp(command, "MUL") == 0)
        return MUL;

    if (strcmp(command, "IN") == 0)
        return IN;

    if (strcmp(command, "OUT") == 0)
        return OUT;

    if (strcmp(command, "END") == 0)
        return END;

    ColoredPrintf(RED, "SYNTAX ERROR\n");
    return WRONG;
}


void ProсessCommand(cmdName_t command)
{
    switch(command) 
    {
    case START:
        DoStart();
        break;

    case PUSH:
        DoPush();
        break;

    case ADD:
        DoAdd();
        break;

    case SUB:
        DoSub();
        break;

    case DIV:
        DoDiv();
        break;

    case MUL:
        DoMul();
        break;

    case IN:
        DoIn();
        break;
    
    case OUT:
        DoOut();
        break;

    case END:
        DoEnd();
        break;

    case WRONG:

    default:
        ColoredPrintf(RED, "SYNTAX ERROR\n");
        DoEnd();
    }
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
    ProсessCommand((cmdName_t) processor->machineCode[processor->instructionNum]);
    processor->instructionNum++;
}


static void DoStart()
{
    STACK_CREATE(stack, sizeof(int));
}

static void DoPush()
{
    int elem = 0;
    if (scanf("%d", &elem) <= 0)
    {
        ColoredPrintf(RED, "PUSH ERROR\n");
        return;
    }
    
    StackPush(stack, &elem);
}


static void DoAdd()
{
    DO_OPERATION(+);
}


static void DoSub()
{
    DO_OPERATION(-);
}


static void DoDiv()
{
    DO_OPERATION(/);
}


static void DoMul()
{
    DO_OPERATION(*);
}


static void DoIn()
{
    ColoredPrintf(YELLOW, "IN\n");
}


static void DoOut()
{
    int lastElem = 0;
    if (StackPop(stack, &lastElem) != OK)
    {
        ColoredPrintf(RED, "OUT: POP ERROR\n");
        return;
    }

    printf(">%d\n", lastElem);
}


static void DoEnd()
{
    StackDelete(&stack);
}
