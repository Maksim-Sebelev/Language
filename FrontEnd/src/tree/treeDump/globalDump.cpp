#include <stdio.h>
#include <assert.h>
#include "tree/tree.hpp"
#include "tree/treeDump/globalDump.hpp"

//=============================== GLobal Graphic Dump Function ==============================================================================================================================

void DotEnd(FILE* dotFile)
{
    assert(dotFile);
    fprintf(dotFile, "}\n");
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DotCreateDumpPlace(FILE* dotFile, const char* file, const int line, const char* func)
{
    assert(dotFile);
    assert(file);
    assert(func);

    fprintf(dotFile, "place");
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#1771a0\", pos=\"0,1!\",");
    fprintf(dotFile, "label = \" { Dump place: | file: [%s] | line: [%d] | func: [%s] } \"", file, line, func);
    fprintf(dotFile, "color = \"#000000\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetTypeInStr(Type type)
{
    for (size_t i = 0; i < DefaultTypesQuant; i++)
        RETURN_IF_TRUE(type == DefaultTypes[i].value, DefaultTypes[i].nameInfo.name);
    
    assert(0 && "undef type");
    return "undefined";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetOperationInStr(Operation oper)
{
    for (size_t i = 0; i < DefaultOperationsQuant; i++)
        RETURN_IF_TRUE(oper == DefaultOperations[i].value, DefaultOperations[i].nameInfo.name);

    assert(0 && "you forgot about some operation.");
    return "wtf?";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetSeparatorInStr(Separator separator)
{
    for (size_t i = 0; i < DefaultSeparatorsQuant; i++)
        RETURN_IF_TRUE(separator == DefaultSeparators[i].value, DefaultSeparators[i].nameInfo.name);

    assert(0 && "you forgot about some operation.");
    return "wtf?";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetBracketInStr(Bracket bracket)
{
    for (size_t i = 0; i < DefaultBracketsQuant; i++)
        RETURN_IF_TRUE(bracket == DefaultBrackets[i].value, DefaultBrackets[i].nameInfo.name);

    assert(0 && "you forgot about some operation.");
    return "wtf?";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetCycleInStr(Cycle cycle)
{
    for (size_t i = 0; i < DefaultCyclesQuant; i++)
        RETURN_IF_TRUE(cycle == DefaultCycles[i].value, DefaultCycles[i].nameInfo.name);

    assert(0 && "you forgot about some operation.");
    return "wtf?";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetConditionInStr(Condition condition)
{
    for (size_t i = 0; i < DefaultConditionsQuant; i++)
        RETURN_IF_TRUE(condition == DefaultConditions[i].value, DefaultConditions[i].nameInfo.name);

    assert(0 && "you forgot about some operation.");
    return "wtf?";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool IsDoubleEqual(double firstNum, double secondNum, double epss)
{
    double difference = firstNum - secondNum;

    return  (difference <=  epss) &&
            (difference >= -epss);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void FprintNumber(FILE* dotFile, Number number)
{
    assert(dotFile);

    Type type = number.type;

    switch (type)
    {
        case Type::int_type:    fprintf(dotFile, "%d", number.value.int_val);    break;
        case Type::double_type: fprintf(dotFile, "%f", number.value.double_val); break;
        case Type::char_type:   fprintf(dotFile, "%c", number.value.char_val);   break;
        case Type::void_type:
        case Type::undefined_type:
        default: assert(0 && "something went wrong");
    }
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void FprintName(FILE* dotFile, Name name)
{
    assert(dotFile);
 
    // Name        name    = table.data[pointer];
    const char* nameStr = name.name.name;
    size_t      nameLen = name.name.len;

    for (size_t i = 0; i < nameLen; i++)
    {
        fprintf(dotFile, "%c", nameStr[i]);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
