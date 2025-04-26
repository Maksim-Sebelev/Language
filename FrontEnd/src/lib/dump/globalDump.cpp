#include <stdio.h>
#include <assert.h>
#include "tree/tree.hpp"
#include "globalDump.hpp"


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
    fprintf(dotFile, "label = \" { Dump place: | file: [%s] | line: [%d] | func: [%s] | autor: Maksimka | I'm not gay } \"", file, line, func);
    fprintf(dotFile, "color = \"#000000\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetOperationInStr(Operation oper)
{
    for (size_t i = 0; i < DefaultOperationsQuant; i++)
    {
        RETURN_IF_TRUE(oper == DefaultOperations[i].value, DefaultOperations[i].name);
    }

    assert(0 && "you forgot about some operation.\n");
    return "wtf?";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* GetFuncInStr(Function func)
{
    for (size_t i = 0; i < DefaultFunctionsQuant; i++)
    {
        RETURN_IF_TRUE(func == DefaultFunctions[i].value, DefaultFunctions[i].name);
    }

    assert(0 && "You forgot abourt some function in graphic dump.\n"); 
    return "undefined";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool IsDoubleEqual(double firstNum, double secondNum, double epss)
{
    double difference = firstNum - secondNum;

    return  (difference <=  epss) &&
            (difference >= -epss);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
