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

    Assemble("test.asm");

    LOG_CLOSE();
    return 0;
}