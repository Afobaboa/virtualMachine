#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/assembler.h"
#include "../headers/processor.h"
#include "../stack/logPrinter/logPrinter.h"


//----------------------------------------------------------------------------------------


enum COMMAND_GET_STATUS
{
    OK,
    NO_CMD,
    WRONG_CMD
};
typedef enum COMMAND_GET_STATUS cmdGetStatus_t;


//----------------------------------------------------------------------------------------


static bool CheckAsmFileExtension(const char* fileName);


static void SetObjectFileExtension(char* fileName);


static bool AssembleCommands(FILE* fileToAssemble, FILE* assembledFile);


static cmdGetStatus_t CommandGet(FILE* fileToAssemble, command_t* cmd, int* cmdArgv);


static bool CommandAssembledWrite(FILE* assembledFile, command_t cmd, int* cmdArgv);


//----------------------------------------------------------------------------------------


bool Assemble(const char* fileName) 
{
    if (fileName == NULL)
    {
        ColoredPrintf(RED, "You trying to assemble file with NULL ptr.\n");
        return false;
    }

    if (!CheckAsmFileExtension(fileName))
    {
        ColoredPrintf(RED, "Wrong file extension.\n");
        return false;
    }

    FILE* fileToAsseble = fopen(fileName, "r");
    if (fileToAsseble == NULL)
    {
        ColoredPrintf(RED, "Can't open file. Check if file exists.\n");
        return false;
    }

    const size_t nameLength = strlen(fileName);
    char* assembledFileName = (char*) calloc(nameLength + 1, sizeof(char));
    if (assembledFileName == NULL)
    {
        ColoredPrintf(RED, "Can't allocate memory for object file name.");
        fclose(fileToAsseble);
        return false;
    }
    memmove(assembledFileName, fileName, nameLength);
    SetObjectFileExtension(assembledFileName);
    FILE* assembledFile = fopen(assembledFileName, "w");
    if (assembledFile == NULL)
    {
        ColoredPrintf(RED, "Can't create object file.\n");
        fclose(fileToAsseble);
        free(assembledFileName);
        return false;
    }

    bool assemblingResult = AssembleCommands(fileToAsseble, assembledFile);

    fclose(fileToAsseble);
    fclose(assembledFile);
    free(assembledFileName);
    return assemblingResult;
}


//----------------------------------------------------------------------------------------


static bool CheckAsmFileExtension(const char* fileName)
{
    const char*  extension       = ".asm";
    const size_t nameLength      = strlen(fileName);
    const size_t extensionLength = strlen(extension);

    if (nameLength <= extensionLength)
        return false;

    if (strcmp(fileName + nameLength - extensionLength, extension) != 0)
        return false;

    return true;
}


static void SetObjectFileExtension(char* fileName)
{
    const char* prevExtencion = ".asm";
    const char* newExtencion  = ".vm";

    const size_t prevExtencionLength = strlen(prevExtencion);
    const size_t newExtencionLength  = strlen(newExtencion);
    const size_t nameLength          = strlen(fileName);
    
    memmove(fileName + nameLength - prevExtencionLength, 
            newExtencion, 
            newExtencionLength + 1);
}


static bool AssembleCommands(FILE* fileToAssemble, FILE* assembledFile)
{
    command_t cmdBuffer = WRONG;
    int cmdArgvBuffer[maxCmdArgc] = {};
    cmdGetStatus_t cmdGetStatus = OK;

    for (;;)
    {
        cmdGetStatus = CommandGet(fileToAssemble, &cmdBuffer, cmdArgvBuffer);
        if (cmdGetStatus == NO_CMD)
            break;

        if ( cmdGetStatus == WRONG_CMD ||
            !CommandAssembledWrite(assembledFile, cmdBuffer, cmdArgvBuffer))
        {
            return false;
        }
    }

    return true;
}


static cmdGetStatus_t CommandGet(FILE* fileToAssemble, command_t* cmd, int* cmdArgv)
{
    char cmd[maxCmdLength] = {};
    char* cmdScanfFormat = CmdScanfFormatGet();
    if (scanf(cmdScanfFormat, cmd) == EOF)
    {
        free(cmdScanfFormat);
        return NO_CMD;
    }
    if (CheckIfScannedCorrect(cmd))
    {
        ColoredPrintf(RED, "Wrong command!\n");
        free(cmdScanfFormat);
        return WRONG_CMD;
    }

    const size_t maxCmdLineLength = 100;
    char cmdLine[maxCmdLineLength] = {};
    if (fgets(cmdLine, max)

    if (strcmp((const char*) cmd, "PUSH") == 0)
    {
        *cmd = PUSH;

    }

    free(cmdScanfFormat);
    return OK;
}


static bool CommandAssembledWrite(FILE* assembledFile, command_t cmd, int* cmdArgv)
{

}