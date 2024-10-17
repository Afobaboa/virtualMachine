#ifndef LABEL_ARRAY_H
#define LABEL_ARRAY_H


//----------------------------------------------------------------------------------------


#include "stddef.h"


//----------------------------------------------------------------------------------------


const size_t maxLabelNameLength = 32;
const size_t poisonLabelNum     = (size_t) -1;

struct Label
{
    char*  labelName;
    size_t instructionNum;
};


const size_t maxLabelCount = 128;

struct LabelArray
{
    Label* labelData;
    size_t labelCount;
};


//----------------------------------------------------------------------------------------


bool LabelArrayCreate(LabelArray* labelArray);


bool LabelArrayDelete(LabelArray* labelArray);


void LabelArrayDump(LabelArray* labelArray, 
                    const char* fileName, const char* functionName, const size_t lineNum);

                
#define LABEL_ARRAY_DUMP(labelArrayPtr) \
    LabelArrayDump(labelArrayPtr, __FILE__, __FUNCTION__, __LINE__)


bool LabelAdd(LabelArray* labelArray, char* labelName, const size_t instructionNum);


bool LabelFind(LabelArray* labelArray, char* labelName, size_t* instructionNumBuffer);


//----------------------------------------------------------------------------------------


#endif // LABEL_ARRAY_H
