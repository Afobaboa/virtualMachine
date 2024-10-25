                             ///////////////
/////////////////////////////// PUSH, POP //////////////////////////////////////////////////////////
                             ///////////////

#define PROCESS_IF_REGISTER_(pushPopMode, argBuffer)    \
    if (IsRegister(argBuffer))                          \
    {                                                   \
        pushPopMode.isRegister = true;                  \
        argv[argc++] = AToRegisterName(argBuffer);      \
    }

#define PROCESS_IF_CONST_(pushPopMode, argBuffer)           \
    if (ConvertToInstruction(argBuffer, &nextInstruction))  \
    {                                                       \
        pushPopMode.isConst = true;                         \
        argv[argc++] = nextInstruction;                     \
    }

#define PROCESS_ARG_(pushPopMode, argBuffer)        \
{                                                   \
    instruction_t nextInstruction = 0;              \
                                                    \
    PROCESS_IF_REGISTER_(pushPopMode, argBuffer)    \
    else                                            \
        PROCESS_IF_CONST_(pushPopMode, argBuffer)   \
    else                                            \
        return CMD_WRONG;                           \
}

#define DO_IF_PLUS_(CODE)                   \
{                                           \
    SkipSpaces(assembler);                  \
    if (assembler->assemblyCode[0] == '+')  \
    {                                       \
        assembler->assemblyCode++;          \
        CODE;                               \
    }                                       \
}

#define GET_NEXT_WORD_()                                \
{                                                       \
    if (GetNextWord(assembler, argBuffer) != CMD_OK)    \
        return CMD_WRONG;                               \
}


#define PROCESS_PUSH_POP_WITHOUT_RAM_(pushPopMode)  \
{                                                   \
    PROCESS_ARG_(pushPopMode, argBuffer);           \
    DO_IF_PLUS_(                                    \
    {                                               \
        GET_NEXT_WORD_();                           \
        PROCESS_ARG_(pushPopMode, argBuffer);       \
    })                                              \
}


#define PROCESS_PUSH_POP_WITH_RAM_TEMPLATE_(pushPopMode)    \
{                                                           \
    char* newArgBuffer = NULL;                              \
    if (argBuffer[0] == '[')                                \
        newArgBuffer = argBuffer + 1;                       \
    else                                                    \
        newArgBuffer = argBuffer;                           \
                                                            \
    if (newArgBuffer[strlen(newArgBuffer) - 1] == ']')      \
    {                                                       \
        newArgBuffer[strlen(newArgBuffer) - 1] = '\0';      \
    }                                                       \
    PROCESS_ARG_(pushPopMode, newArgBuffer);                \
}

#define PROCESS_PUSH_POP_WITH_RAM_(pushPopMode)             \
{                                                           \
    pushPopMode.isRAM = true;                               \
    PROCESS_PUSH_POP_WITH_RAM_TEMPLATE_(pushPopMode);       \
    DO_IF_PLUS_(                                            \
    {                                                       \
        GET_NEXT_WORD_();                                   \
        PROCESS_PUSH_POP_WITH_RAM_TEMPLATE_(pushPopMode);   \
    })                                                      \
}

#define SET_PUSH_POP_(CMD_NAME, pushPopMode)                                                \
{                                                                                           \
    char argBuffer[MAX_CMD_LENGTH + 1] = {};                                                \
    instruction_t argv[4] = {};                                                             \
    size_t argc = 0;                                                                        \
                                                                                            \
    MachineCodeAddInstruction(&assembler->machineCode, (instruction_t) CMD_NAME);           \
                                                                                            \
    GET_NEXT_WORD_();                                                                       \
    if (argBuffer[0] == '[')                                                                \
    {                                                                                       \
        PROCESS_PUSH_POP_WITH_RAM_(pushPopMode);                                            \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
        PROCESS_PUSH_POP_WITHOUT_RAM_(pushPopMode);                                         \
    }                                                                                       \
                                                                                            \
    MachineCodeAddInstruction(&assembler->machineCode, *((instruction_t*) &pushPopMode));   \
    for (size_t argNum = 0; argNum < argc; argNum++)                                        \
        MachineCodeAddInstruction(&assembler->machineCode, argv[argNum]);                   \
}


//////////
// PUSH //
//////////

DEF_CMD_(PUSH, 
{
    PushPopMode pushMode = {};
    SET_PUSH_POP_(PUSH, pushMode);
    return CMD_OK;
},
{
    PushPopMode pushMode = {};
    MachineCodeGetNextInstruction(&processor->machineCode, (instruction_t*) &pushMode);
    
    instruction_t result = 0;
    if (pushMode.isRegister)
    {
        instruction_t registerName = 0;
        MachineCodeGetNextInstruction(&processor->machineCode, &registerName);
        result += RegisterGetValue(&processor->registers, (registerName_t) registerName);
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
})


/////////
// POP //
/////////

DEF_CMD_(POP,
{
    PushPopMode popMode = {};
    SET_PUSH_POP_(POP, popMode);
    if (!popMode.isRAM && popMode.isConst)
        return CMD_WRONG;
    
    return CMD_OK;
},
{
    PushPopMode popMode = {};
    MachineCodeGetNextInstruction(&processor->machineCode, (instruction_t*) &popMode);
    
    instruction_t nextInstruction = 0;
    if (popMode.isRAM)
    {
        size_t cellNum = 0;
        if (popMode.isRegister)
        {
            MachineCodeGetNextInstruction(&processor->machineCode, &nextInstruction);
            cellNum += RegisterGetValue(&processor->registers, (registerName_t) nextInstruction);
        }

        if (popMode.isConst)
        {
            MachineCodeGetNextInstruction(&processor->machineCode, &nextInstruction);
            cellNum += nextInstruction;
        }

        instruction_t value = 0;
        if (StackPop(processor->stack, &value) != OK)
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
})

#undef DO_IF_PLUS_
#undef PROCESS_ARG_
#undef PROCESS_IF_CONST_
#undef PROCESS_IF_REGISTER_

#undef GET_NEXT_WORD_

#undef PROCESS_PUSH_POP_WITH_RAM_
#undef PROCESS_PUSH_POP_WITHOUT_RAM_

#undef SET_PUSH_POP_



                                /////////////////////////
                                // ADD, SUB, MUL, DIV, //
////////////////////////////////// IN, OUT,            /////////////////////////////////////////////
                                // DRAW,               //
                                // RET                 //
                                /////////////////////////

#define SET_CMD_NO_ARGS_(CMD_NAME)                                                  \
{                                                                                   \
    MachineCodeAddInstruction(&assembler->machineCode, (instruction_t) CMD_NAME);   \
    SkipSpaces(assembler);                                                          \
    SkipComments(assembler);                                                        \
    return CMD_OK;                                                                  \
}

#define DO_OPERATION_(operation)                                        \
{                                                                       \
    instruction_t firstPoppedElem  = 0;                                 \
    instruction_t secondPoppedElem = 0;                                 \
    if ((StackPop(processor->stack, &firstPoppedElem)  != OK) ||        \
        (StackPop(processor->stack, &secondPoppedElem) != OK))          \
    {                                                                   \
        LOG_PRINT(INFO, "Stack ptr = %p\n", processor->stack);          \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);            \
        return false;                                                   \
    }                                                                   \
                                                                        \
    instruction_t result = firstPoppedElem operation secondPoppedElem;  \
    StackPush(processor->stack, &result);                               \
}


////////////////////////
// ADD, SUB, MUL, DIV //
//////////////////////// 

DEF_CMD_(ADD,  SET_CMD_NO_ARGS_(ADD), DO_OPERATION_(+))
DEF_CMD_(SUB,  SET_CMD_NO_ARGS_(SUB), DO_OPERATION_(-))
DEF_CMD_(MUL,  SET_CMD_NO_ARGS_(MUL), DO_OPERATION_(*))
DEF_CMD_(DIV,  SET_CMD_NO_ARGS_(DIV), DO_OPERATION_(/))
#undef DO_OPERATION_


////////
// IN //
////////

DEF_CMD_(IN, SET_CMD_NO_ARGS_(IN),
{
    instruction_t inputNum = 0;
    if (scanf("%zu", &inputNum) <= 0)
        return false;

    StackPush(processor->stack, &inputNum);
})


/////////
// OUT //
/////////

DEF_CMD_(OUT, SET_CMD_NO_ARGS_(OUT),
{
    instruction_t lastElem = 0;
    if (StackPop(processor->stack, &lastElem) != OK)
    {
        ColoredPrintf(RED, "OUT: POP ERROR\n");
        return false;
    }

    ColoredPrintf(YELLOW, "%d\n", lastElem);
})


//////////
// DRAW //
//////////

DEF_CMD_(DRAW, SET_CMD_NO_ARGS_(DRAW),
{
    RamScreenDraw(&processor->ram);
})


/////////
// RET //
/////////

DEF_CMD_(RET, SET_CMD_NO_ARGS_(RET),
{
    instruction_t instructionNum = 0;
    StackPop(processor->callStack, &instructionNum);
    MachineCodeJump(&processor->machineCode, JUMP_ABSOLUTE, (int64_t) instructionNum);
})
#undef SET_CMD_NO_ARGS_


                            /////////////////////////////////////
////////////////////////////// JMP, JA, JAE, JB, JBE, JE, JNE, /////////////////////////////////////
                            // CALL                            //
                            /////////////////////////////////////

#define SET_JUMP_(JUMP_NAME)                                        \
{                                                                   \
    MachineCodeAddInstruction(&assembler->machineCode, JUMP_NAME);  \
    return JumpGetAndWriteAddress(assembler);                       \
}

#define DO_JUMP_()                                                                          \
{                                                                                           \
    instruction_t instructionNum = 0;                                                       \
    MachineCodeGetNextInstruction(&(processor->machineCode), &instructionNum);              \
    MachineCodeJump(&(processor->machineCode), JUMP_ABSOLUTE, (int64_t) instructionNum);    \
}

#define DO_JUMP_IF_(CONDITION)                                  \
{                                                               \
    instruction_t lastInstruction    = 0;                       \
    instruction_t preLastInstruction = 0;                       \
                                                                \
    if (StackPop(processor->stack, &lastInstruction) != OK ||   \
        StackPop(processor->stack, &preLastInstruction) != OK)  \
    {                                                           \
        ColoredPrintf(RED, "%s: POP ERROR\n", __FUNCTION__);    \
        return false;                                           \
    }                                                           \
                                                                \
    if (lastInstruction CONDITION preLastInstruction)           \
    {                                                           \
        DO_JUMP_();                                             \
    }                                                           \
    else                                                        \
        MachineCodeSkipInstruction(&processor->machineCode);    \
                                                                \
    StackPush(processor->stack, &preLastInstruction);           \
    StackPush(processor->stack, &lastInstruction);              \
}


////////////////////////////////////
// JMP, JA, JAE, JB, JBE, JE, JNE //
////////////////////////////////////

DEF_CMD_(JMP, SET_JUMP_(JMP), DO_JUMP_())
DEF_CMD_(JA,  SET_JUMP_(JA),  DO_JUMP_IF_(>))
DEF_CMD_(JAE, SET_JUMP_(JAE), DO_JUMP_IF_(>=))
DEF_CMD_(JB,  SET_JUMP_(JB),  DO_JUMP_IF_(<))
DEF_CMD_(JBE, SET_JUMP_(JBE), DO_JUMP_IF_(<=))
DEF_CMD_(JE,  SET_JUMP_(JE),  DO_JUMP_IF_(==))
DEF_CMD_(JNE, SET_JUMP_(JNE), DO_JUMP_IF_(!=))


//////////
// CALL //
//////////

DEF_CMD_(CALL, SET_JUMP_(CALL),
{
    instruction_t thisInstructionEnd = MachineCodeGetInstructionNum(&processor->machineCode) + 1;
    StackPush(processor->callStack, &thisInstructionEnd);
    DO_JUMP_();
})

#undef SET_JUMP_
#undef DO_JUMP_IF_
#undef DO_JUMP_