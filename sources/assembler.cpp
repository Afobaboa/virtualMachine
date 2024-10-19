#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "virtualMachine.h"
#include "machineCode.h"
#include "labelArray.h"
#include "logPrinter.h"
#include "fileProcessor.h"


//--------------------------------------------------------------------------------------------------


/**
 * This statuses are used in functions which are trying to get command from .asm file.
 */
enum COMMAND_GET_STATUS
{
    CMD_OK,         /**< Next command is readed.                  */
    CMD_NO,         /**< There are no commands left in .asm file. */
    CMD_WRONG,      /**< Command doesn't exist.                   */
    CMD_LABEL
};
typedef enum COMMAND_GET_STATUS cmdGetStatus_t;


//--------------------------------------------------------------------------------------------------


static bool AssembleCmds(char* assemblyCode, MachineCode* machineCode, LabelArray* labelArray);


static cmdGetStatus_t GetNextWord(char** contentPtr, char* wordBuffer, size_t* lineNum);


/** 
 * This function check if symbol is space symbol.
 * Space symbols: ' ', '\t', '\n'.
 * 
 * @param symbol Symbol.
 * 
 * @return true if symbol is a space symbol,
 * @return false else.
 */
static bool IsSpace(char symbol);


/**
 * This functin check if symbol is a comment symbol.
 * Comment symbol is ';'.
 * 
 * @param symbol Symbol.
 * 
 * @return true if symbol is a comment symbol,
 * @return false else.
 */
static bool IsCommentSymbol(char symbol);


static void SkipSpaces(char** contentPtr, size_t* lineNum);


static void SkipComments(char** contentPtr, size_t* lineNum);


/**
 * This function converts null-terminated string to decimal int and save value in 
 * intBuffer. This funciton don't check if string is to long, and there may be overflow
 * of int value.
 * 
 * @param string    Null-terminated string.
 * @param intBuffer Ptr to buffer of int value.
 * 
 * @return true if convertion is complete,
 * @return false if string isn't a number in char's notation.
 */
static bool ConvertToInt(char* string, int* intBuffer);


/** 
 * This function convert cmdGetStatus to string with the same name (CMD_OK will be 
 * converted to "CMD_OK"). This function may be usefull for debugging.
 * 
 * @param cmdGetStatus Your cmdGetStatus you want to know.
 * 
 * @return Name of cmdGetStatus. If it isn't exist, that will contains in returned string.
 */
static const char* CmdGetStatusName(cmdGetStatus_t cmdGetStatus);


static cmdGetStatus_t JumpSetAddress(char** assemblyCode, size_t* jumpInstructionNumBuffer, 
                                     LabelArray* labelArray, size_t* lineNum);


static cmdGetStatus_t CmdNextGetAndWrite(char** assemblyCodePtr, MachineCode* machineCode, 
                                         LabelArray* labelArray, size_t* lineNum);


//--------------------------------------------------------------------------------------------------


bool Assemble(const char* fileName) 
{
    if (fileName == NULL)
    {
        ColoredPrintf(RED, "You trying to assemble file with NULL ptr.\n");
        return false;
    }
    if (!FileNameCheckExtension(fileName, ".asm"))
    {
        ColoredPrintf(RED, "Wrong file extension.\n");
        return false;
    }

    MachineCode machineCode = {};
    if (!MachineCodeInit(&machineCode))
    {
        ColoredPrintf(RED, "Can't init machine code.\n");
        return false;
    }
    LabelArray labelArray = {};
    if (!LABEL_ARRAY_CREATE(&labelArray))
    {
        ColoredPrintf(RED, "Can't create label array.\n");
        MachineCodeDelete(&machineCode);
        return false;
    }
    char* fileToAssembleContent = NULL;
    if (!FileGetContent(fileName, &fileToAssembleContent))
    {
        ColoredPrintf(RED, "Assembler error: can't read content of %s.\n", fileName);
        MachineCodeDelete(&machineCode);
        LabelArrayDelete(&labelArray);
        return false;
    }

    LABEL_ARRAY_DUMP(&labelArray);
    AssembleCmds(fileToAssembleContent, &machineCode, &labelArray);
    LABEL_ARRAY_DUMP(&labelArray);
    MachineCodeJump(&machineCode, JUMP_ABSOLUTE, FIRST_INSTRUCTION_NUM);
    bool assemblingResult = AssembleCmds(fileToAssembleContent, &machineCode, &labelArray);
    LABEL_ARRAY_DUMP(&labelArray);

    char* assembledFileName = NULL;
    if (!FileNameChangeExtension((char*) fileName, &assembledFileName, ".asm", 
                                                                    MACHINE_CODE_FILE_EXTENSION))
    {
        ColoredPrintf(RED, "Can't set assembledFileName.\n");
        MachineCodeDelete(&machineCode);
        LabelArrayDelete(&labelArray);
        free(fileToAssembleContent);
        return false;
    }
    assemblingResult = MachineCodeWriteToFile(&machineCode, assembledFileName);

    MachineCodeDelete(&machineCode);
    LabelArrayDelete(&labelArray);
    free(fileToAssembleContent);
    free(assembledFileName);
    return assemblingResult;
}


//--------------------------------------------------------------------------------------------------


static bool AssembleCmds(char* assemblyCode, MachineCode* machineCode, LabelArray* labelArray)
{
    size_t lineNum = 1;
    cmdGetStatus_t cmdGetStatus = CMD_WRONG;

    for (;;)
    {
        cmdGetStatus = CmdNextGetAndWrite(&assemblyCode, machineCode, labelArray, &lineNum);

        if (cmdGetStatus == CMD_NO)
            break;

        if (cmdGetStatus == CMD_WRONG)
            return false;
    }

    return true;
}


static cmdGetStatus_t GetNextWord(char** contentPtr, char* wordBuffer, size_t* lineNum)
{
    SkipSpaces(contentPtr, lineNum);

    int nextChar = 0;
    for (size_t charNum = 0; charNum < MAX_CMD_LENGTH; charNum++)
    {
        // printf("contentPtr = %p\n", contentPtr);
        // printf("content = %p\n", *contentPtr);
        nextChar = **contentPtr;
        if (nextChar == '\0')
        {
            if (charNum == 0)
                return CMD_NO;
            
            break;
        }

        if (IsSpace((char) nextChar) || IsCommentSymbol((char) nextChar))
            break;

        wordBuffer[charNum] = (char) nextChar;
        (*contentPtr)++;
    }

    nextChar = **contentPtr;
    if (nextChar != '\0' && !IsSpace((char) nextChar) && !IsCommentSymbol((char) nextChar))
    {
        // LOG_PRINT(INFO, "cmd = <%s>, nextChar = %d\n", cmdNameBuffer, nextChar);
        return CMD_WRONG;
    }
    // wordBuffer[MAX_CMD_LENGTH] = '\0';

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


static void SkipSpaces(char** contentPtr, size_t* lineNum)
{
    int nextChar = 0;
    for (;;)
    {
        nextChar = **contentPtr;
        
        if (nextChar == '\0')
            return;
            
        if (nextChar == '\n')
            (*lineNum)++;

        if (!IsSpace((char) nextChar))
            break;

        (*contentPtr)++;
    }
}


static void SkipComments(char** contentPtr, size_t* lineNum)
{
    int nextChar = **contentPtr;
    if (nextChar == '\0')
        return;

    if (IsCommentSymbol((char) nextChar))
        for (;;)
        {
            (*contentPtr)++;
            nextChar = **contentPtr;
            if (nextChar == '\n')
            {
                (*lineNum)++;
                break;
            }
        }
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

    case CMD_LABEL:
        return  GET_NAME(CMD_LABEL);

    default:
        char* errorMessage = (char*) calloc (64, sizeof(char));
        sprintf(errorMessage, "cmdGetStatus %d doesn't exist.\n", cmdGetStatus);
        return errorMessage;
    }
}


static cmdGetStatus_t JumpSetAddress(char** assemblyCode, size_t* jumpInstructionNumBuffer, 
                                     LabelArray* labelArray, size_t* lineNum)
{
    char labelName[maxLabelNameLength] = {};
    GetNextWord(assemblyCode, labelName, lineNum);
    if (!LabelIs(labelName))
    {
        LOG_PRINT(ERROR, "LabelName = <%s>\n", labelName);
        return CMD_WRONG;
    }

    size_t instructionNum = labelPoisonNum;
    LabelFind(labelArray, labelName, &instructionNum);
    if (instructionNum == labelPoisonNum)
    {
        LabelAdd(labelArray, labelName, instructionNum);
        return CMD_WRONG;
    }

    *jumpInstructionNumBuffer = instructionNum;
    
    return CMD_OK;
}


#define CMD_SET_CASE(CMD_NAME)                                                  \
{                                                                               \
    if (strcmp(cmdName, GET_NAME(CMD_NAME)) == 0)                               \
    {                                                                           \
        MachineCodeAddInstruction(machineCode, (instruction_t) CMD_NAME);       \
        char argBuffer[MAX_CMD_LENGTH + 1] = {};                                \
        for (size_t argNum = 0; argNum < (size_t) CMD_NAME##_ARGC; argNum++)    \
        {                                                                       \
            cmdGetStatus = GetNextWord(assemblyCodePtr, argBuffer, lineNum);    \
            if (cmdGetStatus != CMD_OK)                                         \
            {                                                                   \
                ColoredPrintf(RED, "Error in line %zu: "                        \
                                   "wrong arguments of command %s.\n",          \
                                    *lineNum,                                   \
                                    GET_NAME(CMD_NAME));                        \
                return cmdGetStatus;                                            \
            }                                                                   \
                                                                                \
            if (!ConvertToInt(argBuffer, (int*) (argvBuffer + argNum)))         \
                return CMD_WRONG;                                               \
                                                                                \
            MachineCodeAddInstruction(machineCode, argvBuffer[argNum]);         \
        }                                                                       \
        SkipSpaces(assemblyCodePtr, lineNum);                                   \
        SkipComments(assemblyCodePtr, lineNum);                                 \
                                                                                \
        return CMD_OK;                                                          \
    }                                                                           \
}


static cmdGetStatus_t CmdNextGetAndWrite(char** assemblyCodePtr, MachineCode* machineCode, 
                                         LabelArray* labelArray, size_t* lineNum)
{
    char cmdName[MAX_CMD_LENGTH + 1] = {};
    cmdGetStatus_t cmdGetStatus = GetNextWord(assemblyCodePtr, cmdName, lineNum);
    // LOG_PRINT(INFO, "cmdName = <%s>.\n", cmdName);

    if (cmdGetStatus == CMD_WRONG)
    {
        ColoredPrintf(RED, "Error in line %zu: wrong command name!\n", *lineNum);
        // LOG_PRINT(ERROR, "cmd <%s> is wrong.\n", cmdName);
        return CMD_WRONG;
    }
    if (cmdGetStatus == CMD_NO)
        return CMD_NO;

    if (LabelIs(cmdName))
    {
        LabelAdd(labelArray, cmdName, MachineCodeGetInstructionNum(machineCode));
        return CMD_LABEL;
    }

    instruction_t argvBuffer[MAX_CMD_ARGC + 1] = {};
    CMD_SET_CASE(PUSH);
    CMD_SET_CASE(ADD);
    CMD_SET_CASE(SUB);
    CMD_SET_CASE(DIV);    
    CMD_SET_CASE(MUL);
    CMD_SET_CASE(OUT);
    CMD_SET_CASE(IN);

    if (strcmp(cmdName, "JMP") == 0)
    {
        MachineCodeAddInstruction(machineCode, JMP);
        size_t jumpInstructionNum = 0;
        JumpSetAddress(assemblyCodePtr, &jumpInstructionNum, labelArray, lineNum);
        
        MachineCodeAddInstruction(machineCode, jumpInstructionNum);
        return CMD_OK;
    }

    ColoredPrintf(RED, "Error in line %zu: command %s doesn't exist.\n", 
                        lineNum, cmdName);
    return CMD_WRONG;
}
#undef CMD_SET_CASE
