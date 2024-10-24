#ifndef REGISTER_64_H
#define REGISTER_64_H


//--------------------------------------------------------------------------------------------------


#include "machineCode.h"


//--------------------------------------------------------------------------------------------------


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


typedef instruction_t register64_t;

#define DEF_REGISTER_(registerName) \
    register64_t registerName;

struct Registers64
{
    #include "registers.h"
};
#undef DEF_REGISTER_


//--------------------------------------------------------------------------------------------------


bool IsRegister(char* string);


registerName_t AToRegisterName(char* string);


instruction_t RegisterGetValue(Registers64* registers, registerName_t registerName);


//--------------------------------------------------------------------------------------------------


#endif // REGISTER_64_H