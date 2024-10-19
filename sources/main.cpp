#include "assembler.h"
#include "processor.h"
#include "labelArray.h"


//--------------------------------------------------------------------------------------------------


int main() 
{
    LOG_OPEN();

    if (!Assemble("test.asm"))
    {
        ColoredPrintf(RED, "Assembling failed\n");
        LOG_CLOSE();
        return 1;
    }

    if (!ExecuteProgram("test.vm"))
        ColoredPrintf(RED, "Executing failed\n");

    LOG_CLOSE();
    return 0;
}