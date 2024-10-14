#ifndef PROCESSOR_H
#define PROCESSOR_H


//----------------------------------------------------------------------------------------


 #include "../stack/logPrinter/logPrinter.h"


 //---------------------------------------------------------------------------------------


/**
 * Codes of virtual machine's commands.
 */
enum COMMAND_CODES
{
    START = 1,
    PUSH,         /**< Push elem to stack. */
    ADD,
    SUB,
    DIV,
    MUL,
    IN,
    OUT,
    END,
    WRONG
};
typedef enum COMMAND_CODES command_t;


/**
 * Arguments' count of each command of virtualMachine.
 */
typedef int argc_t;
argc_t START_ARGC = 0;
argc_t PUSH_ARGC  = 1;
argc_t ADD_ARGC   = 0;
argc_t SUB_ARGC   = 0;
argc_t DIV_ARGC   = 0;
argc_t MUL_ARGC   = 0;
argc_t IN_ARGC    = 0;
argc_t OUT_ARGC   = 0;
argc_t END_ARGC   = 0;

const size_t maxCmdArgc = 5;


//----------------------------------------------------------------------------------------


command_t GetCommand();


void ProсessCommand(command_t command);


//----------------------------------------------------------------------------------------


#endif // PROCESSOR_H