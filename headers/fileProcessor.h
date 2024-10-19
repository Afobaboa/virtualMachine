#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H


//--------------------------------------------------------------------------------------------------


bool FileNameCheckExtension(const char* fileName, const char* extension);


bool FileNameChangeExtension(char* prevFileName, char** newFileNameBuffer,
                             const char* prevExtension, const char* newExtension);


bool FileMarkAsBeated(char* fileName);


bool FileGetSize(char* filename, size_t* sizeBuffer);


bool FileGetContent(const char* fileName, char** contentBufferPtr);


//--------------------------------------------------------------------------------------------------


#endif // FILE_PROCESSOR_H