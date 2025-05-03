#include <stdio.h>
#include <assert.h>
#include "lib/lib.hpp"
#include "lib/colorPrint.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintPlace(const char* file, const int line, const char* func)
{
    assert(file);
    assert(func);

    COLOR_PRINT(WHITE, "%s:%d\n", file, line);
    COLOR_PRINT(WHITE, "function '");
    COLOR_PRINT(GREEN, "%s", func);
    COLOR_PRINT(WHITE, "'\n");

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CodePlaceCtor(CodePlace* place, const char* const file, const int line, const char* const func)
{
    assert(place);
    assert(file);
    assert(func);

    place->file = file;
    place->line = line;
    place->func = func;

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
