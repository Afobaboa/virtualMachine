#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/assembler.h"
#include "../headers/processor.h"
#include "../stack/logPrinter/logPrinter.h"


//----------------------------------------------------------------------------------------


#define CMD_SET(CMD_NAME)\
    *cmdName = CMD_NAME;


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


static bool AssembleCmds(FILE* fileToAssemble, FILE* assembledFile);


static cmdGetStatus_t CmdGet(FILE* fileToAssemble, cmdName_t* cmdNameBuffer, 
                                                   int*       cmdArgvBuffer);


static cmdGetStatus_t CmdGetName(FILE* fileGetFrom, char* cmdNameBuffer);


static bool CmdAssembledWrite(FILE* assembledFile, cmdName_t cmd, int* cmdArgv);


bool IsSpace(char symbol);


bool IsCommentSymbol(char symbol);


void SkipSpaces(FILE* file);


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

    bool assemblingResult = AssembleCmds(fileToAsseble, assembledFile);

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


static bool AssembleCmds(FILE* fileToAssemble, FILE* assembledFile)
{
    cmdName_t cmdBuffer = WRONG;
    int cmdArgvBuffer[maxCmdArgc] = {};
    cmdGetStatus_t cmdGetStatus = OK;

    for (;;)
    {
        cmdGetStatus = CmdGet(fileToAssemble, &cmdBuffer, cmdArgvBuffer);
        if (cmdGetStatus == NO_CMD)
            break;

        if ( cmdGetStatus == WRONG_CMD ||
            !CmdAssembledWrite(assembledFile, cmdBuffer, cmdArgvBuffer))
        {
            return false;
        }
    }

    return true;
}


static cmdGetStatus_t CmdGet(FILE* fileToAssemble, cmdName_t* cmdNameBuffer, 
                                                   int*       cmdArgvBuffer)
{
    char cmdName[maxCmdNameLength + 1] = {};
    cmdGetStatus_t cmdGetStatus = CmdGetName(fileToAssemble, cmdName);

    if (cmdGetStatus == WRONG_CMD)
    {
        ColoredPrintf(RED, "Wrong command name!\n");
        return WRONG_CMD;
    }
    if (cmdGetStatus == NO_CMD)
        return NO_CMD;

    if (strcmp(cmdName, "PUSH"))
        CMD_SET(PUSH);

    return OK;
}


static bool CmdAssembledWrite(FILE* assembledFile, cmdName_t cmd, int* cmdArgv)
{
    
}


static cmdGetStatus_t CmdGetName(FILE* fileGetFrom, char* cmdNameBuffer)
{
    SkipSpaces(fileGetFrom);

    int nextChar = 0;
    for (size_t charNum = 0; charNum < maxCmdNameLength; charNum++)
    {
        nextChar = fgetc(fileGetFrom);
        if (nextChar == EOF)
        {
            if (charNum == 0)
                return NO_CMD;
            
            break;
        }

        if (IsSpace(nextChar) || IsCommentSymbol(nextChar))
        {
            ungetc(nextChar, fileGetFrom);
            break;
        }

        cmdNameBuffer[charNum] = nextChar;
    }

    nextChar = fgetc(fileGetFrom);
    if (nextChar != EOF || !IsSpace(nextChar) || !IsCommentSymbol(nextChar))
    {
        ungetc(nextChar, fileGetFrom);
        return WRONG_CMD;
    }
    ungetc(nextChar, fileGetFrom);
    cmdNameBuffer[maxCmdNameLength] = '\0';

    return OK;
}


bool IsSpace(char symbol)
{
    if (symbol == ' ' || symbol == '\t' || symbol == '\n')
        return true;

    return false;
}


bool IsCommentSymbol(char symbol)
{
    if (symbol == ';')
        return true;
    
    return false;
}


void SkipSpaces(FILE* file)
{
    int nextChar = 0;
    for (;;)
    {
        nextChar = fgetc(file);
        
        if (nextChar == EOF)
            return;

        if (!IsSpace(nextChar))
            break;
    }
    ungetc(nextChar, file);
}
