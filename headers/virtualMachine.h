#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H


//--------------------------------------------------------------------------------------------------


#include "machineCode.h"
#include "register64.h"


//--------------------------------------------------------------------------------------------------


struct PushPopMode
{
    instruction_t isConst    : 1;
    instruction_t isRegister : 1;
    instruction_t isRAM      : 1;
};


//--------------------------------------------------------------------------------------------------


#define DEF_CMD_(cmdName, ...) \
    , cmdName

enum COMMAND_NAMES
{
    CMD_NAME_WRONG
    #include "commands.h"
};
typedef enum COMMAND_NAMES cmdName_t;
#undef DEF_CMD_

const size_t MAX_CMD_LENGTH = 32;


//--------------------------------------------------------------------------------------------------


#endif // VIRTUAL_MACHINE_H