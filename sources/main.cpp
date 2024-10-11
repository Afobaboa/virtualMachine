#include "../headers/processor.h"


//----------------------------------------------------------------------------------------


int main() 
{
    command_t command = WRONG;

    for (;;)
    {
        command = GetCommand();
        ProсessCommand(command);
        
        if (command == HLT)
        break;
    }

    return 0;
}