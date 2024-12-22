#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "../../../Common/Dump/GlobalDump.hpp"
#include "../NameTable.hpp"
#include "NameTableDump.hpp"


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void NameTableDumpHelper(const char* dotFileName, const NameTable_t* table, const char* file, const int line, const char* func);
static void DotBegin(FILE* dotFile);
static void DotCreateNode(FILE* dotFile, const NameTable_t* table, size_t pointer);
static void DotCreateAllNames(FILE* dotFile, const NameTable_t* table);
static const char* GetNameColor(Name name);
static const char* GetNameTypeInStr(Name name);
static void FprintfName(FILE* dotFile, Name name);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NameTableDump(const NameTable_t* table, const char* file, const int line, const char* func)
{
    assert(table);
    assert(file);
    assert(func);

    static size_t ImgQuant = 1;

    static const size_t MaxfileNameLen = 128;
    char outfile[MaxfileNameLen] = {};
    sprintf(outfile, "tree%lu.png", ImgQuant);
    ImgQuant++;

    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    static const char* dotFileName = "tree.dot";
    sprintf(command, "dot -Tpng %s > %s", dotFileName, outfile);

    NameTableDumpHelper(outfile, table, file, line, func);
    system(command);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void NameTableDumpHelper(const char* dotFileName, const NameTable_t* table, const char* file, const int line, const char* func)
{
    assert(dotFileName);
    assert(table);
    assert(file);
    assert(func);

    FILE* dotFile = fopen(dotFileName, "wb");
    assert(dotFile);

    DotBegin            (dotFile);
    DotCreateDumpPlace  (dotFile, file, line, func);    
    DotCreateAllNames   (dotFile, table);
    DotEnd              (dotFile);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotBegin(FILE* dotFile)
{
    assert(dotFile);

    fprintf(dotFile, "digraph G{\nrankdir=TB\ngraph [bgcolor=\"#000000\"];\nsize = \"100 5\"\n");
    
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateNode(FILE* dotFile, const NameTable_t* table, size_t pointer)
{
    assert(dotFile);
    assert(table);

    Name name = table->data[pointer];

    const char* nameColor = GetNameColor(name);
    const char* nameType  = GetNameTypeInStr(name);
    size_t      nameLen  = name.nameLen;


    fprintf(dotFile, "node%lu ", pointer);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"%s\", ", nameColor);
    fprintf(dotFile, "label = \"{ | ");
    fprintf(dotFile, "%s | ", nameType);
    FprintfName(dotFile, name);
    fprintf(dotFile, " | %lu", nameLen);
    fprintf(dotFile, "}\"\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateAllNames(FILE* dotFile, const NameTable_t* table)
{
    assert(dotFile);
    assert(table);

    size_t size = table->size;

    for (size_t namePointer = 0; namePointer < size; namePointer++)
    {
        DotCreateNode(dotFile, table, namePointer);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetNameColor(Name name)
{
    NameType type = name.type;

    switch (type)
    {
        case NameType::Variable: return "#f1481a";
        case NameType::Function: return "#f31807";
        default: assert(0 && "you forgot about some name type");
    }

    assert(0 && "wtf?");
    return "undefined";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetNameTypeInStr(Name name)
{
    NameType type = name.type;

    switch (type)
    {
        case NameType::Variable: return "variable";
        case NameType::Function: return "function";
        default: assert(0 && "you forgot about some name type");
    }

    assert(0 && "wtf?");
    return "undefined";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void FprintfName(FILE* dotFile, Name name)
{
    assert(dotFile);

    const char* nameStr = name.name;
    size_t      nameLen = name.nameLen;

    for (size_t namePointer = 0; namePointer < nameLen; namePointer++)
    {
        fprintf(dotFile, "%c", nameStr[namePointer]);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
