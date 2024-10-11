#include "../headers/processor.h"


//----------------------------------------------------------------------------------------


int main() 
{
    LOG_OPEN();

    command_t command = WRONG;

    for (;;)
    {
        command = GetCommand();
        ProсessCommand(command);
        
        if (command == HLT)
        break;
    }

    LOG_CLOSE();
    return 0;
}