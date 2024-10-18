#ifndef LABEL_ARRAY_H
#define LABEL_ARRAY_H


//----------------------------------------------------------------------------------------


#include "logPrinter.h"


//----------------------------------------------------------------------------------------


const size_t maxLabelNameLength = 32;
const size_t labelPoisonNum     = (size_t) -1;

struct Label
{
    char   name[maxLabelNameLength + 1];
    size_t instructionNum;
};


const size_t maxLabelCount = 128;

struct LabelArray
{
    Label* data;
    size_t labelCount;
};


//----------------------------------------------------------------------------------------


bool LabelArrayCreate(LabelArray* labelArray, Place place);

#define LABEL_ARRAY_CREATE(labelArray) \
    LabelArrayCreate(labelArray, GET_PLACE())


void LabelArrayDelete(LabelArray* labelArray);


void LabelArrayDump(LabelArray* labelArray, Place place);

#define LABEL_ARRAY_DUMP(labelArrayPtr) \
    LabelArrayDump(labelArrayPtr, GET_PLACE())


bool LabelAdd(LabelArray* labelArray, char* labelName, const size_t instructionNum);


bool LabelFind(LabelArray* labelArray, char* labelName, size_t* instructionNumBuffer);


bool LabelIs(char* labelName);


//----------------------------------------------------------------------------------------


#endif // LABEL_ARRAY_H
