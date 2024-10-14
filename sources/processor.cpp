 #include <stdio.h>
 #include <string.h>

 #include "../headers/processor.h"
 #include "../stack/headers/stack.h"


//----------------------------------------------------------------------------------------


#define DO_OPERATION(operation)                                 \
    int elem = 0;                                               \
    if (scanf("%d", &elem) <= 0)                                \
    {                                                           \
        ColoredPrintf(RED, "%s ERROR\n", __FUNCTION__);         \
        return;                                                 \
    }                                                           \
                                                                \
    int poppedElem = 0;                                         \
    if (StackPop(stack, &poppedElem) != OK)                     \
    {                                                           \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);    \
        return;                                                 \
    }                                                           \
                                                                \
    poppedElem = poppedElem operation elem;                     \
    StackPush(stack, &poppedElem);                              


//----------------------------------------------------------------------------------------


static Stack* stack = NULL;


//----------------------------------------------------------------------------------------


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


command_t GetCommand()
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


void ProсessCommand(command_t command)
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
