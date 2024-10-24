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


#define DEF_REGISTER_(registerName) \
    , registerName

enum REGISTER_NAMES
{
    REGISTER_NAME_WRONG
    #include "registers.h"
};
typedef enum REGISTER_NAMES registerName_t;
#undef DEF_REGISTER_


//--------------------------------------------------------------------------------------------------


#endif // VIRTUAL_MACHINE_H