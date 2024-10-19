#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H


//----------------------------------------------------------------------------------------


#include <stddef.h>


//----------------------------------------------------------------------------------------


/**
 * Codes of virtual machine's commands.
 */
enum COMMAND_NAMES
{
    PUSH = 1,         /**< Push elem to stack. */
    ADD,
    SUB,
    DIV,
    MUL,
    IN,
    OUT,
    JMP,
    WRONG
};
typedef enum COMMAND_NAMES cmdName_t;

const size_t maxCmdLength = 32;


/**
 * Arguments' count of each command of virtualMachine.
 */
typedef int argc_t;
const argc_t PUSH_ARGC = 1;
const argc_t ADD_ARGC  = 0;
const argc_t SUB_ARGC  = 0;
const argc_t DIV_ARGC  = 0;
const argc_t MUL_ARGC  = 0;
const argc_t IN_ARGC   = 0;
const argc_t OUT_ARGC  = 0;
const argc_t JMP_ARGC  = 1;

const size_t maxCmdArgc = 4;


//----------------------------------------------------------------------------------------


#endif // VIRTUAL_MACHINE_H