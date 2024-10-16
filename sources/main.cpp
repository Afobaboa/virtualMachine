#include "../headers/assembler.h"
#include "../headers/processor.h"


//----------------------------------------------------------------------------------------


int main() 
{
    LOG_OPEN();

    // cmdName_t command = WRONG;

    // for (;;)
    // {
    //     command = GetCommand();
    //     ColoredPrintf(WHITE, "%d\n", command);
    //     ProсessCommand(command);
        
    //     if (command == END || command == WRONG)
    //     break;
    // }

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