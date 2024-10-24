#include <string.h>

#include "register64.h"
#include "logPrinter.h"


//--------------------------------------------------------------------------------------------------


#define DEF_REGISTER_(REGISTER_NAME)                    \
{                                                       \
    if (strcmp(string, GET_NAME(REGISTER_NAME)) == 0)   \
        return true;                                    \
}

bool IsRegister(char* string)
{
    #include "registers.h"

    // else
    return false;    
}
#undef DEF_REGISTER_


#define DEF_REGISTER_(REGISTER_NAME)                   \
{                                                       \
    if (strcmp(string, GET_NAME(REGISTER_NAME)) == 0)   \
        return REGISTER_NAME;                           \
}

registerName_t AToRegisterName(char* string)
{
    #include "registers.h"

    // else
    return REGISTER_NAME_WRONG;
}
#undef DEF_REGISTER_


#define DEF_REGISTER_(REGISTER_NAME) \
{\
    if (registerName == REGISTER_NAME)\
        return registers->REGISTER_NAME;\
}

instruction_t RegisterGetValue(Registers64* registers, registerName_t registerName)
{
    #include "registers.h"

    // else
    LOG_PRINT(ERROR, "WRONG REGISTER NAME\n");
    return 0;
}
#undef DEF_REGISTER_
