 #include <stdio.h>
 #include <string.h>

 #include "../headers/processor.h"
 #include "../stack/headers/stack.h"


//----------------------------------------------------------------------------------------


static void DoPush();
static void DoAdd();
static void DoSub();
static void DoDiv();
static void DoMul();
static void DoIn();
static void DoOut();
static void DoHlt(); 


//----------------------------------------------------------------------------------------


command_t GetCommand()
{
    const size_t maxCommandSize = 10;
    char command[maxCommandSize];
    scanf("%s", command);

    if (command[maxCommandSize - 1] != '\0')
    {
        ColoredPrintf(RED, "SYNTAX ERROR\n");
        return WRONG;
    }

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

    if (strcmp(command, "HLT") == 0)
        return HLT;

    ColoredPrintf(RED, "SYNTAX ERROR\n");
    return WRONG;
}


void ProсessCommand(command_t command)
{
    switch(command) 
    {
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

    case HLT:
        DoHlt();
        break;

    case WRONG:

    default:
        ColoredPrintf(RED, "SYNTAX ERROR\n");
        DoHlt();
    }
}


//----------------------------------------------------------------------------------------


static void DoPush()
{
    ColoredPrintf(YELLOW, "PUSH\n");
}


static void DoAdd()
{
    ColoredPrintf(YELLOW, "ADD\n");
}


static void DoSub()
{
    ColoredPrintf(YELLOW, "SUB\n");
}


static void DoDiv()
{
    ColoredPrintf(YELLOW, "DIV\n");
}


static void DoMul()
{
    ColoredPrintf(YELLOW, "MUL\n");
}


static void DoIn()
{
    ColoredPrintf(YELLOW, "IN\n");
}


static void DoOut()
{
    ColoredPrintf(YELLOW, "OUT\n");
}


static void DoHlt()
{
    ColoredPrintf(YELLOW, "HLT\n");
}
