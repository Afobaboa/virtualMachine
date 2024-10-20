#ifndef LABEL_ARRAY_H
#define LABEL_ARRAY_H


//--------------------------------------------------------------------------------------------------


#include "logPrinter.h"


//--------------------------------------------------------------------------------------------------


const size_t MAX_LABEL_NAME_LENGTH = 32;
const size_t LABEL_POISON_NUM      = (size_t) -1;
const size_t LABEL_DUMMY_NUM       = (size_t) -2;


struct Label
{
    char   name[MAX_LABEL_NAME_LENGTH + 1];
    size_t instructionNum;
};


const size_t MAX_LABEL_COUNT = 128;

struct LabelArray
{
    Label* data;
    size_t labelCount;
};


//--------------------------------------------------------------------------------------------------


bool LabelArrayCreate(LabelArray* labelArray, Place place);

#define LABEL_ARRAY_CREATE(labelArray) \
    LabelArrayCreate(labelArray, GET_PLACE())


void LabelArrayDelete(LabelArray* labelArray);


void LabelArrayDump(LabelArray* labelArray, Place place);

#define LABEL_ARRAY_DUMP(labelArrayPtr) \
    LabelArrayDump(labelArrayPtr, GET_PLACE())


bool LabelAdd(LabelArray* labelArray, char* labelName, const size_t instructionNum);


bool LabelChangeInstructionNum(LabelArray* labelArray, char* labelName, 
                               const size_t newInstructionNum);


bool LabelAddOrChangeInstructionNum(LabelArray* labelArray, char* labelName,
                                    const size_t newInstructionNum);


bool LabelFind(LabelArray* labelArray, char* labelName, size_t* instructionNumBuffer);


bool LabelIs(char* labelName);


//--------------------------------------------------------------------------------------------------


#endif // LABEL_ARRAY_H
