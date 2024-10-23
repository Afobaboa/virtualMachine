#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H


//--------------------------------------------------------------------------------------------------


#include "machineCode.h"


//--------------------------------------------------------------------------------------------------


struct PushPopMode
{
    instruction_t isConst    : 1;
    instruction_t isRegister : 1;
    instruction_t isRAM      : 1;
};


//--------------------------------------------------------------------------------------------------


/**
 * Codes of virtual machine's commands.
 */
enum COMMAND_NAMES
{
    PUSH = 1,         /**< Push elem to stack. */
    POP,
    ADD,
    SUB,
    DIV,
    MUL,
    IN,
    OUT,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JNE,
    DRAW,
    WRONG
};
typedef enum COMMAND_NAMES cmdName_t;

const size_t MAX_CMD_LENGTH = 32;


enum REGISTER_NAMES
{
    RAX,
    RBX,
    RCX, 
    RDX
};
typedef enum REGISTER_NAMES registerName_t;


//--------------------------------------------------------------------------------------------------


#endif // VIRTUAL_MACHINE_H