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
        
        if (command == END || command == WRONG)
        break;
    }

    LOG_CLOSE();
    return 0;
}