#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/assembler.h"
#include "../headers/processor.h"
#include "../stack/logPrinter/logPrinter.h"


//----------------------------------------------------------------------------------------


static bool CheckAsmFileExtension(const char* fileName);


static void SetObjectFileExtension(char* fileName);


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

    fprintf(assembledFile, "Test\n");

    fclose(fileToAsseble);
    fclose(assembledFile);
    free(assembledFileName);
    return true;
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