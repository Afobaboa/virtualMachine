#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/assembler.h"
#include "../headers/processor.h"
#include "../stack/logPrinter/logPrinter.h"


//----------------------------------------------------------------------------------------


#define CMD_SET(CMD_NAME)                                                       \
{                                                                               \
    *cmdNameBuffer = CMD_NAME;                                                  \
    char argBuffer[maxCmdLength + 1] = {};                                      \
    for (size_t argNum = 0; argNum < (size_t) CMD_NAME##_ARGC; argNum++)        \
    {                                                                           \
        cmdGetStatus = GetNextWord(fileToAssemble, argBuffer);                  \
        if (cmdGetStatus != OK)                                                 \
            return WRONG_CMD;                                                   \
                                                                                \
        if (!ConvertToInt(argBuffer, cmdArgvBuffer + argNum))                   \
            return WRONG_CMD;                                                   \
    }                                                                           \
}


#define CMD_ASSEMBLED_WRITE(CMD_NAME)                                       \
{                                                                           \
    fprintf(assembledFile, "%d ", CMD_NAME);                                \
    for (size_t argNum = 0; argNum < (size_t) CMD_NAME##_ARGC; argNum++)    \
    {                                                                       \
        fprintf(assembledFile, "%d ", cmdArgv[argNum]);                     \
    }                                                                       \
}


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


static bool AssembleCmds(FILE* fileToAssemble, FILE* assembledFile);


static cmdGetStatus_t CmdGet(FILE* fileToAssemble, cmdName_t* cmdNameBuffer, 
                                                   int*       cmdArgvBuffer);


static cmdGetStatus_t GetNextWord(FILE* fileGetFrom, char* cmdNameBuffer);


static bool CmdAssembledWrite(FILE* assembledFile, cmdName_t cmd, int* cmdArgv);


bool IsSpace(char symbol);


bool IsCommentSymbol(char symbol);


void SkipSpaces(FILE* file);


void SkipComments(FILE* file);


bool ConvertToInt(char* string, int* intBuffer);


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
    char cmdName[maxCmdLength + 1] = {};
    cmdGetStatus_t cmdGetStatus = GetNextWord(fileToAssemble, cmdName);

    if (cmdGetStatus == WRONG_CMD)
    {
        ColoredPrintf(RED, "Wrong command name!\n");
        return WRONG_CMD;
    }
    if (cmdGetStatus == NO_CMD)
        return NO_CMD;

    if (strcmp(cmdName, "PUSH"))
    {
        CMD_SET(PUSH);
    }

    else if (strcmp(cmdName, "ADD"))
    {
        CMD_SET(ADD);
    }

    else if (strcmp(cmdName, "SUB"))
    {
        CMD_SET(SUB);
    }

    else if (strcmp(cmdName, "DIV"))
    {
        CMD_SET(DIV);
    }    

    else if (strcmp(cmdName, "MUL"))
    {
        CMD_SET(MUL);
    }

    else if (strcmp(cmdName, "OUT"))
    {
        CMD_SET(OUT);
    }

    else if (strcmp(cmdName, "IN"))
    {
        CMD_SET(IN);
    }

    else 
        return WRONG_CMD;

    return OK;
}


static bool CmdAssembledWrite(FILE* assembledFile, cmdName_t cmd, int* cmdArgv)
{
    if (assembledFile == NULL || cmdArgv == NULL)
        return false;

    switch (cmd)
    {
    case PUSH:
        CMD_ASSEMBLED_WRITE(PUSH);
        break;

    case ADD:
        CMD_ASSEMBLED_WRITE(ADD);
        break;

    case SUB:
        CMD_ASSEMBLED_WRITE(SUB);
        break;

    case DIV:
        CMD_ASSEMBLED_WRITE(DIV);
        break;

    case MUL:
        CMD_ASSEMBLED_WRITE(MUL);
        break;

    case OUT:
        CMD_ASSEMBLED_WRITE(OUT);
        break;

    case IN:
        CMD_ASSEMBLED_WRITE(IN);
        break;
    }

    return true;
}


static cmdGetStatus_t GetNextWord(FILE* fileGetFrom, char* cmdNameBuffer)
{
    SkipSpaces(fileGetFrom);

    int nextChar = 0;
    for (size_t charNum = 0; charNum < maxCmdLength; charNum++)
    {
        nextChar = fgetc(fileGetFrom);
        if (nextChar == EOF)
        {
            if (charNum == 0)
                return NO_CMD;
            
            break;
        }

        if (IsSpace((char) nextChar) || IsCommentSymbol((char) nextChar))
        {
            ungetc(nextChar, fileGetFrom);
            break;
        }

        cmdNameBuffer[charNum] = (char) nextChar;
    }

    nextChar = fgetc(fileGetFrom);
    if (nextChar != EOF || !IsSpace((char) nextChar) || !IsCommentSymbol((char) nextChar))
    {
        ungetc(nextChar, fileGetFrom);
        return WRONG_CMD;
    }
    ungetc(nextChar, fileGetFrom);
    cmdNameBuffer[maxCmdLength] = '\0';

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

        if (!IsSpace((char) nextChar))
            break;
    }
    ungetc(nextChar, file);
}


void SkipComments(FILE* file)
{
    int nextChar = fgetc(file);
    if (nextChar == EOF)
        return;

    if (IsCommentSymbol((char) nextChar))
        for (;;)
        {
            nextChar = fgetc(file);
            if (nextChar == '\n')
                break;
        }
    
    else    
        ungetc(nextChar, file);
}


bool ConvertToInt(char* string, int* intBuffer)
{
    int value = 0;
    const size_t digitCount = strlen(string);
    for(size_t digitNum = 0; digitNum < digitCount; digitNum++)
    {
        char nextDigit = string[digitNum];
        if (!isdigit(nextDigit))
            return false;
        
        if (digitCount > 1 && digitNum == 0 && nextDigit == '0')
            return false;

        value = value*10 + (nextDigit - '0');
    }

    *intBuffer = value;
    return true;
}
