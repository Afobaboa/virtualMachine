#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "labelArray.h"


//----------------------------------------------------------------------------------------


static bool LabelArrayVerify(LabelArray* labelArray);


static bool LabelVerify(Label* label);


static bool LabelNameVerify(Label* label);


static void LabelArraySetPoison(LabelArray* labelArray);


static void LabelDump(Label* label);


//----------------------------------------------------------------------------------------


bool LabelArrayCreate(LabelArray* labelArray, Place place)
{
    labelArray->labelCount = 0;

    labelArray->data = (Label*) calloc(maxLabelCount, sizeof(Label));
    if (labelArray->data == NULL)
    {
        LOG_PRINT(ERROR, "%s: %s(): line %d: labelData == NULL\n",
                  place.file, place.function, place.line);
        return false;
    }

    LabelArraySetPoison(labelArray);

    return true;
}


void LabelArrayDelete(LabelArray* labelArray)
{
    free(labelArray->data);
    
    labelArray->data  = NULL;
    labelArray->labelCount = 0;
}


void LabelArrayDump(LabelArray* labelArray, Place place)
{
    LOG_PRINT(INFO, "%s: %s(): line %d: LabelArray dumping...\n", 
              place.file, place.function, place.line);
    LOG_DUMMY_PRINT("\tlabelCount = %zu\n"
                    "\tlabelData  = %p\n", 
                    labelArray->labelCount,
                    labelArray->data);

    if (labelArray->data != NULL)
    {
        LOG_DUMMY_PRINT("Labels:\n");
        char* format = GetArrayPrintingFormat(place, labelArray->labelCount);

        for (size_t labelNum = 0; labelNum < labelArray->labelCount; labelNum++)
        {
            LOG_DUMMY_PRINT(format, labelNum);
            LOG_DUMMY_PRINT("\n");
            LabelDump(labelArray->data + labelNum);
            LOG_DUMMY_PRINT("\n");
        }

        free(format);
    }
}


bool LabelAdd(LabelArray* labelArray, char* labelName, const size_t instructionNum)
{
    if (labelName == NULL)
    {
        LOG_PRINT(ERROR, "labelName == NULL\n");
        return false;
    }
    
    if (instructionNum == labelPoisonNum)
    {
        LOG_PRINT(ERROR, "instructionNum == POSON == %zu\n", labelPoisonNum);
        return false;
    }

    Label* label = labelArray->data + labelArray->labelCount;
    label->instructionNum = instructionNum;

    for (size_t charNum = 0; charNum < maxLabelNameLength; charNum++)
    {
        if (labelName[charNum] == '\0')
            break;
        
        label->name[charNum] = labelName[charNum];
    }

    (labelArray->labelCount)++;
    return true;
}


bool LabelFind(LabelArray* labelArray, char* labelName, size_t* instructionNumBuffer)
{
    if (!LabelArrayVerify(labelArray))
    {
        LABEL_ARRAY_DUMP(labelArray);
        return false;
    }

    for (size_t labelNum = 0; labelNum < labelArray->labelCount; labelNum++)
    {
        Label* label = labelArray->data + labelNum;
        if (strcmp(label->name, labelName) == 0)
        {
            *instructionNumBuffer = label->instructionNum;
            return true;
        }
    }

    return false;
}


bool LabelIs(char* labelName) 
{
    if (labelName[strlen(labelName) - 1] == ':')
        return true;
    
    return false;
}


//----------------------------------------------------------------------------------------


static bool LabelArrayVerify(LabelArray* labelArray)
{
    if (labelArray == NULL)
    {
        LOG_PRINT(ERROR, "labelArray == NULL\n");
        return false;
    }

    if (labelArray->labelCount > maxLabelCount)
    {
        LOG_PRINT(ERROR, "labelCount = %zu is bigger than maxLabelCount = %zu\n",
                  labelArray->labelCount, maxLabelCount);        
        return false;
    }    

    for (size_t labelNum = 0; labelNum < labelArray->labelCount; labelNum++)
        if (!LabelVerify(labelArray->data + labelNum))
        {
            LOG_PRINT(ERROR, "labelNum = %zu isn't verified\n", labelNum);
            return false;
        }

    return true;
}


static bool LabelVerify(Label* label)
{
    if (label == NULL)
    {
        LOG_PRINT(ERROR, "label == NULL");
        return false;
    }

    if (label->instructionNum == labelPoisonNum)
    {
        LOG_PRINT(ERROR, "instructionNum is poisoned, instructionNum == %zu\n", 
                  labelPoisonNum);
        return false;
    }

    if (!LabelNameVerify(label))
    {
        LOG_PRINT(ERROR, "labelName isn't verified!");
        LOG_DUMMY_PRINT("labelName = ");
        for (size_t charNum = 0; charNum <= maxLabelNameLength; charNum++)
        {
            char nextChar = label->name[charNum];
            LOG_DUMMY_PRINT("<%c = %d> ", nextChar, nextChar);
        }

        return false;
    }

    return true;
}


static void LabelArraySetPoison(LabelArray* labelArray)
{
    for (size_t labelNum = 0; labelNum < maxLabelCount; labelNum++)
    {
        Label* label = labelArray->data + labelNum;
        label->instructionNum = labelPoisonNum;
    }
}


static void LabelDump(Label* label)
{
    if (label->name == NULL)
        LOG_DUMMY_PRINT("name == NULL\n");
    else
    {
        for (size_t charNum = 0; charNum < maxLabelNameLength; charNum++)
            LOG_DUMMY_PRINT("%c", label->name[charNum]);
        
        for (size_t charNum = 0; charNum < maxLabelNameLength; charNum++)
            LOG_DUMMY_PRINT("%d ", label->name[charNum]);

        LOG_DUMMY_PRINT("\n");
    }

    if (label->instructionNum == labelPoisonNum)
        LOG_DUMMY_PRINT("***");

    LOG_DUMMY_PRINT("instructionNum = %zu\n", label->instructionNum);
}


static bool LabelNameVerify(Label* label)
{
    if (label->name[maxLabelNameLength] != '\0')
        return false;

    if (label->name[0] == '\0')
        return false;

    return true;
}
