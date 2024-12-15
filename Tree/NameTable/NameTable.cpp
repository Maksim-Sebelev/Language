#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../../Common/ColorPrint.hpp"
#include "../../Common/GlobalInclude.h"
#include "NameTable.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTableErr Verif       (const NameTable* table, NameTableErr* err, const char* file, const int line, const char* func);
static void         PrintError  (                  const NameTableErr* err);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NAME_TABLE_VERIF(table, err) Verif(table, &(err), __FILE__, __LINE__, __func__)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static NameTableCtor(NameTable* table, size_t)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTableErr Verif(const NameTable* table, NameTableErr* err, const char* file, const int line, const char* func)
{
    assert(err);
    assert(file);
    assert(func);

    CodePlaceCtor(&err->place, file, line, func);

    if (!table)
    {
        err->err = NameTableErrorType::TABLE_NULLPTR;
        return *err;
    }

    

    return *err;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintError(const NameTableErr* err)
{
    assert(err);

    NameTableErrorType errType = err->err;

    switch (errType)
    {
        case NameTableErrorType::NO_ERR:
            return;
        
        case NameTableErrorType::TABLE_NULLPTR:
            COLOR_PRINT(RED, "Error: nametable is nullptr, but now is's very bad.\n");
            break;
        
        case NameTableErrorType::CTOR_CALLOC_NULL:
            COLOR_PRINT(RED, "Error: failed allocate memory for nametable.\n");
            break;

    }

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NameTableAssertPrint(const NameTableErr* err, const char* file, const int line, const char* func)
{
    assert(err);
    assert(file);
    assert(func);

    COLOR_PRINT(RED, "Error detected in:\n");
    PrintPlace(file, line, func);
    PrintError(err);
    PrintPlace(err->place.file, err->place.line, err->place.func);
    COLOR_PRINT(CYAN, "abort in 3, 2, 1...\n");

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef NAME_TABLE_VERIF
