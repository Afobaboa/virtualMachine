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
enum COMMAND_STATUS
{
    CMD_OK,         /**< Next command is readed.                  */
    CMD_NO,         /**< There are no commands left in .asm file. */
    CMD_WRONG,      /**< Command doesn't exist.                   */
    CMD_LABEL
};
typedef enum COMMAND_STATUS cmdStatus_t;


//--------------------------------------------------------------------------------------------------


struct Assembler
{
    char* assemblyCode;
    char* firstAssemblyCode;
    MachineCode machineCode;
    LabelArray labelArray;
    size_t lineNum;
};

const size_t FIRST_LINE = 1;


//--------------------------------------------------------------------------------------------------


static bool AssembleCmds(Assembler* assembler);


static cmdStatus_t GetNextWord(Assembler* assembler, char* wordBuffer);


static bool IsSpace(char symbol);
static bool IsCommentSymbol(char symbol);
static void SkipSpaces(Assembler* assembler);
static void SkipComments(Assembler* assembler);


static bool ConvertToInstruction(char* string, instruction_t* valueBuffer);


static cmdStatus_t JumpGetAndWriteAddress(Assembler* assembler);


static cmdStatus_t CmdNextGetAndWrite(Assembler* assembler);


static bool AssemblerInit(Assembler* assembler, const char* fileToAssembleName, Place place);

#define ASSEMBLER_INIT(assembler, fileToAssemble) \
    AssemblerInit(assembler, fileToAssemble, GET_PLACE())

static void AssemblerCodeRewind(Assembler* assembler);
static void AssemblerDelete(Assembler* assembler);


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

    Assembler assembler = {};
    if (!ASSEMBLER_INIT(&assembler, fileName))
    {
        ColoredPrintf(RED, "Can't init assembler.\n");
        return false;
    }

    AssembleCmds(&assembler);
    bool assemblingResult = AssembleCmds(&assembler);

    char* assembledFileName = NULL;
    if (!FileNameChangeExtension((char*) fileName, &assembledFileName, ".asm",
                                                                    MACHINE_CODE_FILE_EXTENSION))
    {
        ColoredPrintf(RED, "Can't set assembledFileName.\n");
        AssemblerDelete(&assembler);
        return false;
    }
    MachineCodeWriteToFile(&assembler.machineCode, assembledFileName);

    AssemblerDelete(&assembler);
    free(assembledFileName);
    return assemblingResult;
}


//--------------------------------------------------------------------------------------------------


static bool AssemblerInit(Assembler* assembler, const char* fileToAssembleName, Place place)
{
    if (!MachineCodeInit(&assembler->machineCode))
    {
        LOG_PRINT_WITH_PLACE(ERROR, place, "Can't init machine code.\n");
        return false;
    }

    if (!LABEL_ARRAY_CREATE(&assembler->labelArray))
    {
        LOG_PRINT_WITH_PLACE(ERROR, place, "Can't create label array.\n");
        MachineCodeDelete(&assembler->machineCode);
        return false;
    }

    if (!FileGetContent(fileToAssembleName, &assembler->assemblyCode))
    {
        LOG_PRINT_WITH_PLACE(ERROR, place, "Assembler error: can't read content of %s.\n", 
                             fileToAssembleName);
        MachineCodeDelete(&assembler->machineCode);
        LabelArrayDelete(&assembler->labelArray);
        return false;
    }
    
    assembler->firstAssemblyCode = assembler->assemblyCode;
    assembler->lineNum = FIRST_LINE;
    return true;
}


static void AssemblerCodeRewind(Assembler* assembler)
{
    assembler->lineNum = FIRST_LINE;
    MachineCodeJump(&assembler->machineCode, JUMP_ABSOLUTE, FIRST_INSTRUCTION_NUM);
    assembler->assemblyCode = assembler->firstAssemblyCode;
}


static void AssemblerDelete(Assembler* assembler)
{
    free(assembler->firstAssemblyCode);
    assembler->firstAssemblyCode = NULL;
    assembler->assemblyCode      = NULL;

    MachineCodeDelete(&assembler->machineCode);
    LabelArrayDelete(&assembler->labelArray);
    assembler->lineNum = 0;
}


static bool AssembleCmds(Assembler* assembler)
{
    AssemblerCodeRewind(assembler);
    cmdStatus_t cmdStatus = CMD_WRONG;

    for (;;)
    {
        cmdStatus = CmdNextGetAndWrite(assembler);

        if (cmdStatus == CMD_NO)
            break;

        if (cmdStatus == CMD_WRONG)
            return false;
    }

    return true;
}


static cmdStatus_t GetNextWord(Assembler* assembler, char* wordBuffer)
{
    SkipSpaces(assembler);

    char nextChar = 0;
    for (size_t charNum = 0; charNum < MAX_CMD_LENGTH; charNum++)
    {
        // printf("contentPtr = %p\n", contentPtr);
        // printf("content = %p\n", *contentPtr);
        nextChar = assembler->assemblyCode[0];
        if (nextChar == '\0')
        {
            if (charNum == 0)
                return CMD_NO;
            
            break;
        }

        if (IsSpace(nextChar) || IsCommentSymbol(nextChar))
            break;

        wordBuffer[charNum] = nextChar;
        wordBuffer[charNum + 1] = '\0';
        assembler->assemblyCode++;
    }

    nextChar = assembler->assemblyCode[0];
    if (nextChar != '\0' && !IsSpace(nextChar) && !IsCommentSymbol(nextChar))
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


static void SkipSpaces(Assembler* assembler)
{
    int nextChar = 0;
    for (;;)
    {
        nextChar = assembler->assemblyCode[0];
        
        if (nextChar == '\0')
            return;
            
        if (nextChar == '\n')
            assembler->lineNum++;

        if (!IsSpace((char) nextChar))
            break;

        assembler->assemblyCode++;
    }
}


static void SkipComments(Assembler* assembler)
{
    int nextChar = assembler->assemblyCode[0];
    if (nextChar == '\0')
        return;

    if (IsCommentSymbol((char) nextChar))
        for (;;)
        {
            assembler->assemblyCode++;
            nextChar = assembler->assemblyCode[0];
            if (nextChar == '\n')
            {
                assembler->lineNum++;
                break;
            }
        }
}


static bool ConvertToInstruction(char* string, instruction_t* valueBuffer)
{
    instruction_t value = 0;
    const size_t digitCount = strlen(string);

    if (strlen(string) == 3 && string[0] == '\'' && string[0] == string[2])
    {
        *valueBuffer = string[1];
        return true;
    }

    for(size_t digitNum = 0; digitNum < digitCount; digitNum++)
    {
        char nextDigit = string[digitNum];
        if (!isdigit(nextDigit))
            return false;
        
        if (digitCount > 1 && digitNum == 0 && nextDigit == '0')
            return false;

        value = value*10 + (instruction_t) (nextDigit - '0');
    }

    *valueBuffer = value;
    return true;
}


static cmdStatus_t JumpGetAndWriteAddress(Assembler* assembler)
{
    char labelName[MAX_LABEL_NAME_LENGTH + 1] = {};
    GetNextWord(assembler, labelName);
    if (!LabelIs(labelName))
    {
        LOG_PRINT(ERROR, "LabelName = <%s>\n", labelName);
        return CMD_WRONG;
    }
    // LABEL_ARRAY_DUMP(&assembler->labelArray);

    size_t instructionNum = LABEL_POISON_NUM;
    LabelFind(&assembler->labelArray, labelName, &instructionNum);
    if (instructionNum == LABEL_POISON_NUM)
    {
        MachineCodeAddInstruction(&assembler->machineCode, LABEL_POISON_NUM);
        LabelAdd(&assembler->labelArray, labelName, LABEL_DUMMY_NUM);
        return CMD_LABEL;
    }

    MachineCodeAddInstruction(&assembler->machineCode, instructionNum);
    
    return CMD_OK;
}


#define DEF_CMD_(CMD_NAME, CMD_SET, ...)            \
{                                                   \
    if (strcmp(cmdName, GET_NAME(CMD_NAME)) == 0)   \
    {                                               \
        CMD_SET;                                    \
    }                                               \
}


static cmdStatus_t CmdNextGetAndWrite(Assembler* assembler)
{
    char cmdName[MAX_CMD_LENGTH + 1] = {};
    cmdStatus_t cmdStatus = GetNextWord(assembler, cmdName);
    // LOG_PRINT(INFO, "cmdName = <%s>.\n", cmdName);

    if (cmdStatus == CMD_WRONG)
    {
        ColoredPrintf(RED, "Error in line %zu: wrong command name!\n", assembler->lineNum);
        // LOG_PRINT(ERROR, "cmd <%s> is wrong.\n", cmdName);
        return CMD_WRONG;
    }
    if (cmdStatus == CMD_NO)
        return CMD_NO;

    if (LabelIs(cmdName))
    {
        size_t instructionNum = MachineCodeGetInstructionNum(&assembler->machineCode);
        LabelAddOrChangeInstructionNum(&assembler->labelArray, cmdName, instructionNum);
        return CMD_LABEL;
    }

    #include "commands.h"

    //else
    ColoredPrintf(RED, "Error in line %zu: command %s doesn't exist.\n", 
                        assembler->lineNum, cmdName);
    return CMD_WRONG;
}
#undef DEF_CMD_
