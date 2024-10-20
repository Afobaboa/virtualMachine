#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "fileProcessor.h"


//--------------------------------------------------------------------------------------------------


bool FileNameCheckExtension(const char* fileName, const char* extension)
{
    const size_t nameLength      = strlen(fileName);
    const size_t extensionLength = strlen(extension);

    if (nameLength <= extensionLength || extension[0] != '.')
        return false;

    if (strcmp(fileName + nameLength - extensionLength, extension) != 0)
        return false;

    return true;
}


bool FileNameChangeExtension(char* prevFileName, char** newFileNameBuffer,
                             const char* prevExtension, const char* newExtension)
{
    if (*newFileNameBuffer != NULL)
        return false;

    const size_t prevExtencionLength = strlen(prevExtension);
    const size_t newExtencionLength  = strlen(newExtension);
    const size_t prevFileNameLength  = strlen(prevFileName);

    if (prevFileNameLength + newExtencionLength < prevExtencionLength)
        return false;
    
    size_t prevFileNameWithoutExtensionLength = prevFileNameLength - prevExtencionLength;
    *newFileNameBuffer = (char*) calloc(prevFileNameWithoutExtensionLength + newExtencionLength + 1, 
                                        sizeof(char));
    if (*newFileNameBuffer == NULL)
        return false;

    memmove(*newFileNameBuffer, prevFileName, prevFileNameWithoutExtensionLength);
    memmove(*newFileNameBuffer + prevFileNameWithoutExtensionLength, 
                                                                newExtension, newExtencionLength);

    return true;
}


bool FileMarkAsBeated(char* fileName)
{
    FILE* file = fopen(fileName, "w");
    if (file == NULL)
        return false;
    
    fprintf(file, "BEATED!!!\n");
    fclose(file);
    return true;;
}


bool FileGetSize(char* fileName, size_t* sizeBuffer)
{
    struct stat fileStat = {};
    if (stat(fileName, &fileStat) == -1)
        return false;

    *sizeBuffer = (size_t) fileStat.st_size;

    return true;
}


bool FileGetContent(const char* fileName, char** contentBufferPtr)
{
    size_t charCount = 0;
    if (!FileGetSize((char*) fileName, &charCount))
        return NULL;
                                            // +1 for make contentBuffer null-terminated
    *contentBufferPtr = (char*) calloc(charCount + 1, sizeof(char));
    if (*contentBufferPtr == NULL)
        return false;;

    FILE* file = fopen(fileName, "rb");
    if (file == NULL)
        return false;

    fread(*contentBufferPtr, sizeof(char), charCount, file);

    fclose(file);
    return true;
}
