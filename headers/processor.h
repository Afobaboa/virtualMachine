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
    PUSH = 1,   /**< Push elem to stack. */
    ADD,
    SUB,
    DIV,
    MUL,
    IN,
    OUT,
    HLT,
    WRONG
};
typedef enum COMMAND_CODES command_t;


//----------------------------------------------------------------------------------------


command_t GetCommand();


void ProсessCommand(command_t command);


//----------------------------------------------------------------------------------------


#endif // PROCESSOR_H