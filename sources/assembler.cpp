#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "processor.h"
#include "logPrinter.h"


//----------------------------------------------------------------------------------------


/**
 * This statuses are used in functions which are trying to get command from .asm file.
 */
enum COMMAND_GET_STATUS
{
    CMD_OK,         /**< Next command is readed.                  */
    CMD_NO,         /**< There are no commands left in .asm file. */
    CMD_WRONG       /**< Command doesn't exist.                   */
};
typedef enum COMMAND_GET_STATUS cmdGetStatus_t;


//----------------------------------------------------------------------------------------


// static size_t lineNum = 0;


//----------------------------------------------------------------------------------------


/**
 * This function check if file's extension is extension you need.
 * The extension is supposed to be at the end of fileName.
 * 
 * @param fileName  Name of file.
 * @param extension Extension you need. 
 *                  Extencion must begin with '.' for correct behavour.
 * 
 * @return true if extension is correct,
 * @return false if it isn't true.
 */
static bool CheckAsmFileExtension(const char* fileName, const char* extension);


/**
 * This function change file extension. 
 * ChangeFileExtension("a.b", ".b", ".c") will change "a.b" to "a.c".
 * This function don't check if fileName have substring prevExtension and 
 * you must start extension names with '.' for correct behavour.
 * 
 * @param fileName      Name of file WITH EXTENCION.
 * @param prevExtencion Extencion you want to change.
 *                      fileName MUST ends with prevExtension!!!
 * @param newExtension  File's extension you want to set. 
 *                      newExtension's length must be <= prevExtension's length.
 * 
 * Because this function don't return error status you should use it very carefully.
 */
static void ChangeFileExtension(char* fileName, const char* prevExtension,
                                                const char* newExtension);


/** 
 * This function convert .asm code from fileToAssemble to machine code of its virtual
 * machine and write it into assembled file. This function check almost all errors. 
 * If there are any detected error in fileToAssemble, this function will mark 
 * assembled file as beated by function MarkFileAsBeated().
 * 
 * @param fileToAssemble Ptr to file with .asm code.
 * @param assembledFile  Ptr to file where will be written machine code.
 * 
 * @return true if converting is complete,
 * @return false if there are any errors. 
 *         This function will be print about errors to terminal.
 *         If you find any detected error the information about this isn't printed to
 *         terminal, make issue about it on github.com/Afobaboa/virtualMachine .
 */
static bool AssembleCmds(FILE* fileToAssemble, FILE* assembledFile);


/**
 * This function tries to get next command from fileToAssemble.
 * This function assumes cmdArgvBuffer is maxCmdArgc, check it yourself.
 * 
 * @param fileToAssemble File with .asm commands for this virtual machine.
 * @param cmdNameBuffer  Ptr to buffer there will be witten name of next command.
 * @param cmdArgvBuffer  Ptr to buffer there will be printed arguments of next command.
 * 
 * @return CMD_OK if all is OK,
 * @return CMD_NO if there are no commands left,
 * @return CMD_WRONG if next command's name of arguments are wrong.
 */
static cmdGetStatus_t CmdGet(FILE* fileToAssemble, cmdName_t* cmdNameBuffer, 
                                                   int*       cmdArgvBuffer);


/**
 * This function get next word from file with .asm commands.
 * Word is a set of chars which are not separated by cpases or comments.
 * 
 * @param fileGetFrom File which you try to get next word from.
 * @param wordBuffer  Ptr to word buffer. Buffer capacity must be not less than 
 *                    maxCmdLength. You must ckeck it yourself.
 * 
 * @return CMD_OK if all is OK,
 * @return CMD_NO if there are not any commands left,
 * @return CMD_WRONG if word from fileGetFrom is too long. 
 *         Ptr to next char of fileGetFrom won't be set in start's position.
 */
static cmdGetStatus_t GetNextWord(FILE* fileGetFrom, char* wordBuffer);


/**
 * This function will write assembled command (machine code of this command) 
 * to assembledFile.
 * 
 * @param assembledFile Ptr to file with machine code.
 * @param cmdName       Name of command.
 * @param cmdArgv       Array with command's arguments.
 * 
 * @return true if writing is comlete,
 * @return false else.
 */
static bool CmdAssembledWrite(FILE* assembledFile, cmdName_t cmdName, int* cmdArgv);


static bool IsSpace(char symbol);


static bool IsCommentSymbol(char symbol);


static void SkipSpaces(FILE* file);


static void SkipComments(FILE* file);


static bool ConvertToInt(char* string, int* intBuffer);


static const char* CmdGetStatusName(cmdGetStatus_t cmdGetStatus);


static bool CopyFileWithHeaderInfo(FILE* fileFrom, FILE* fileTo);


static void MarkFileAsBeated(FILE* file);


static bool GetFileSize(FILE* file, size_t* sizeBuffer);


static size_t CountInstructions(FILE* file);


//----------------------------------------------------------------------------------------


bool Assemble(const char* fileName) 
{
    if (fileName == NULL)
    {
        ColoredPrintf(RED, "You trying to assemble file with NULL ptr.\n");
        return false;
    }

    if (!CheckAsmFileExtension(fileName, ".asm"))
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
    ChangeFileExtension(assembledFileName, ".asm", ".vm");
    FILE* assembledFile = fopen(assembledFileName, "w");
    if (assembledFile == NULL)
    {
        ColoredPrintf(RED, "Can't create object file.\n");
        fclose(fileToAsseble);
        free(assembledFileName);
        return false;
    }
    free(assembledFileName);

    FILE* tempFile = tmpfile();
    if (tempFile == NULL)
    {
        ColoredPrintf(RED, "Can't create temp file.");
        fclose(fileToAsseble);
        fclose(assembledFile);
        return false;
    }

    bool assemblingResult = AssembleCmds(fileToAsseble, tempFile);

    if (assemblingResult == true)
    {
        if (!CopyFileWithHeaderInfo(tempFile, assembledFile))
        {
            ColoredPrintf(RED, "Can't copy content of temp file to assembled file.\n");
            MarkFileAsBeated(assembledFile);
        }
    }
    else
        MarkFileAsBeated(assembledFile);

    fclose(fileToAsseble);
    fclose(assembledFile);
    fclose(tempFile);
    return assemblingResult;
}


//----------------------------------------------------------------------------------------


static bool CheckAsmFileExtension(const char* fileName, const char* extension)
{
    const size_t nameLength      = strlen(fileName);
    const size_t extensionLength = strlen(extension);

    if (nameLength <= extensionLength || extension[0] != '.')
        return false;

    if (strcmp(fileName + nameLength - extensionLength, extension) != 0)
        return false;

    return true;
}


static void ChangeFileExtension(char* fileName, const char* prevExtension,
                                                const char* newExtension)
{
    const size_t prevExtencionLength = strlen(prevExtension);
    const size_t newExtencionLength  = strlen(newExtension);
    const size_t nameLength          = strlen(fileName);
    
    memmove(fileName + nameLength - prevExtencionLength, 
            newExtension, 
            newExtencionLength + 1);
}


static bool AssembleCmds(FILE* fileToAssemble, FILE* assembledFile)
{
    cmdName_t cmdBuffer = WRONG;
    int cmdArgvBuffer[maxCmdArgc] = {};
    cmdGetStatus_t cmdGetStatus = CMD_OK;

    for (;;)
    {
        cmdGetStatus = CmdGet(fileToAssemble, &cmdBuffer, cmdArgvBuffer);

        if (cmdGetStatus == CMD_NO)
            break;

        if ( cmdGetStatus == CMD_WRONG ||
            !CmdAssembledWrite(assembledFile, cmdBuffer, cmdArgvBuffer))
        {
            // LOG_PRINT(ERROR, "cmdGetStatus = %s", CmdGetStatusName(cmdGetStatus));
            return false;
        }
    }

    return true;
}


#define CMD_SET(CMD_NAME)                                                               \
{                                                                                       \
    *cmdNameBuffer = CMD_NAME;                                                          \
    char argBuffer[maxCmdLength + 1] = {};                                              \
    for (size_t argNum = 0; argNum < (size_t) CMD_NAME##_ARGC; argNum++)                \
    {                                                                                   \
        cmdGetStatus = GetNextWord(fileToAssemble, argBuffer);                          \
        if (cmdGetStatus != CMD_OK)                                                     \
        {                                                                               \
            ColoredPrintf(RED, "Wrong arguments of command %s.\n", GET_NAME(CMD_NAME)); \
            return cmdGetStatus;                                                        \
        }                                                                               \
                                                                                        \
                                                                                        \
        if (!ConvertToInt(argBuffer, cmdArgvBuffer + argNum))                           \
            return CMD_WRONG;                                                           \
    }                                                                                   \
    SkipSpaces(fileToAssemble);                                                         \
    SkipComments(fileToAssemble);                                                       \
                                                                                        \
    return CMD_OK;                                                                      \
}


static cmdGetStatus_t CmdGet(FILE* fileToAssemble, cmdName_t* cmdNameBuffer, 
                                                   int*       cmdArgvBuffer)
{
    char cmdName[maxCmdLength + 1] = {};
    cmdGetStatus_t cmdGetStatus = GetNextWord(fileToAssemble, cmdName);
    // LOG_PRINT(INFO, "cmdName = <%s>.\n", cmdName);

    if (cmdGetStatus == CMD_WRONG)
    {
        ColoredPrintf(RED, "Wrong command name!\n");
        // LOG_PRINT(ERROR, "cmd <%s> is wrong.\n", cmdName);
        return CMD_WRONG;
    }
    if (cmdGetStatus == CMD_NO)
        return CMD_NO;

    if (strcmp(cmdName, GET_NAME(PUSH)) == 0)
        CMD_SET(PUSH);

    if (strcmp(cmdName, GET_NAME(ADD)) == 0)
        CMD_SET(ADD);

    if (strcmp(cmdName, GET_NAME(SUB)) == 0)
        CMD_SET(SUB);

    if (strcmp(cmdName, GET_NAME(DIV)) == 0)
        CMD_SET(DIV);    

    if (strcmp(cmdName, GET_NAME(MUL)) == 0)
        CMD_SET(MUL);

    if (strcmp(cmdName, GET_NAME(OUT)) == 0)
        CMD_SET(OUT);

    if (strcmp(cmdName, GET_NAME(IN)) == 0)
        CMD_SET(IN);

    ColoredPrintf(RED, "Command %s doesn't exist.\n", cmdName);
    return CMD_WRONG;
}
#undef CMD_SET


#define CMD_ASSEMBLED_WRITE(CMD_NAME)                                       \
{                                                                           \
    fprintf(assembledFile, "%d ", CMD_NAME);                                \
    for (size_t argNum = 0; argNum < (size_t) CMD_NAME##_ARGC; argNum++)    \
    {                                                                       \
        fprintf(assembledFile, "%d ", cmdArgv[argNum]);                     \
    }                                                                       \
}


static bool CmdAssembledWrite(FILE* assembledFile, cmdName_t cmdName, int* cmdArgv)
{
    if (assembledFile == NULL || cmdArgv == NULL)
        return false;

    switch (cmdName)
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

    case WRONG:
    default:
        LOG_PRINT(ERROR, "cmdName = %d\n", cmdName);
    }

    return true;
}
#undef CMD_ASSEMBLED_WRITE


static cmdGetStatus_t GetNextWord(FILE* fileGetFrom, char* wordBuffer)
{
    SkipSpaces(fileGetFrom);

    int nextChar = 0;
    for (size_t charNum = 0; charNum < maxCmdLength; charNum++)
    {
        nextChar = fgetc(fileGetFrom);
        if (nextChar == EOF)
        {
            if (charNum == 0)
                return CMD_NO;
            
            break;
        }

        if (IsSpace((char) nextChar) || IsCommentSymbol((char) nextChar))
        {
            ungetc(nextChar, fileGetFrom);
            break;
        }

        wordBuffer[charNum] = (char) nextChar;
    }

    nextChar = fgetc(fileGetFrom);
    if (nextChar != EOF && !IsSpace((char) nextChar) && !IsCommentSymbol((char) nextChar))
    {
        // LOG_PRINT(INFO, "cmd = <%s>, nextChar = %d\n", cmdNameBuffer, nextChar);
        ungetc(nextChar, fileGetFrom);
        return CMD_WRONG;
    }
    ungetc(nextChar, fileGetFrom);
    wordBuffer[maxCmdLength] = '\0';

    return CMD_OK;
}


static bool IsSpace(char symbol)
{
    if (symbol == ' ' || symbol == '\t' || symbol == '\n')
        return true;

    return false;
}


static bool IsCommentSymbol(char symbol)
{
    if (symbol == ';')
        return true;
    
    return false;
}


static void SkipSpaces(FILE* file)
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


static void SkipComments(FILE* file)
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


static bool ConvertToInt(char* string, int* intBuffer)
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


static const char* CmdGetStatusName(cmdGetStatus_t cmdGetStatus)
{
    switch (cmdGetStatus)
    {
    case CMD_OK:
        return GET_NAME(CMD_OK);

    case CMD_NO:    
        return GET_NAME(CMD_NO);

    case CMD_WRONG:
        return GET_NAME(CMD_WRONG);

    default:
        char* errorMessage = (char*) calloc (64, sizeof(char));
        sprintf(errorMessage, "cmdGetStatus %d doesn't exist.\n", cmdGetStatus);
        return errorMessage;
    }
}


static bool CopyFileWithHeaderInfo(FILE* fileFrom, FILE* fileTo)
{
    rewind(fileFrom);
    const size_t instructionCount = CountInstructions(fileFrom);
    fprintf(fileTo, "%zu ", instructionCount);

    size_t fileFromSize = 0;
    if (!GetFileSize(fileFrom, &fileFromSize))
    {
        // LOG_PRINT(ERROR, "Can't get size of file\n");
        return false;
    }

    for (size_t charNum = 0; charNum < fileFromSize; charNum++)
    {
        int nextChar = fgetc(fileFrom);
        if (nextChar == EOF)
        {
            // LOG_PRINT(ERROR, "Wrong fileFromSize = %zu, charNum = %zu\n", 
                    //   fileFromSize, charNum);
            return false;
        }

        fputc(nextChar, fileTo);
    }
    
    return true;
}


static void MarkFileAsBeated(FILE* file)
{
    rewind(file);
    fprintf(file, "BEATED!!!\n");
}


static bool GetFileSize(FILE* file, size_t* sizeBuffer)
{
    rewind(file);
    size_t fileSize = 0;

    for (;;)
    {
        int symbol = fgetc(file);
        if (symbol == EOF)
            break;
        
        fileSize++;
    }
    *sizeBuffer = fileSize;
    rewind(file);

    return true;
}


static size_t CountInstructions(FILE* file)
{
    rewind(file);
    int buffer = 0;
    size_t instructionCount = 0;
    for (;;)
    {
        if (fscanf(file, "%d", &buffer) > 0)
            instructionCount++;
        else    
            break;
    }
    rewind(file);

    return instructionCount;
}
