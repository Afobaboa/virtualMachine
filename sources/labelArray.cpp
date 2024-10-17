#include <stdlib.h>

#include "labelArray.h"


//----------------------------------------------------------------------------------------


bool LabelArrayCreate(LabelArray* labelArray);


bool LabelArrayDelete(LabelArray* labelArray);


void LabelArrayDump(LabelArray* labelArray, 
                    const char* fileName, const char* functionName, const size_t lineNum);

                
#define LABEL_ARRAY_DUMP(labelArrayPtr) \
    LabelArrayDump(labelArrayPtr, __FILE__, __FUNCTION__, __LINE__)


bool LabelAdd(LabelArray* labelArray, char* labelName, const size_t instructionNum);


bool LabelFind(LabelArray* labelArray, char* labelName, size_t* instructionNumBuffer)
{
    
}


//----------------------------------------------------------------------------------------
