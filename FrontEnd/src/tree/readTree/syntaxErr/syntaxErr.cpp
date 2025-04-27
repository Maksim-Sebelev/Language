#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lib/colorPrint.hpp"
#include "lib/lib.hpp"
#include "tree/readTree/fileread/fileread.hpp"
#include "tree/readTree/syntaxErr/syntaxErr.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* FindNline        (const char* str, size_t nLine, size_t* lineSize);
static void        PrintNStrSymbols (const char* str, size_t n);
static void        PrintNSpaces     (                 size_t nSpace);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

__attribute__((__noreturn__)) void SyntaxError(size_t errLine, size_t errLinePos, const InputData* inputData, const char* msg, const char* file, const int line, const char* func)
{
    assert(inputData);
    assert(inputData->buffer);
    assert(inputData->inputStream);
    assert(msg);
    assert(file);
    assert(func);

    COLOR_PRINT(RED, "\nSyntaxErr detected in:\n");
    PrintPlace(file, line, func);


    assert(errLine >= 1);
    assert(errLinePos >= 1);


    size_t lineSize = 0;
    const char* LineWithErr = FindNline(inputData->buffer, errLine, &lineSize);


    COLOR_PRINT(WHITE, "\nIn\n%s:%lu:%lu:", inputData->inputStream, errLine, errLinePos);
    printf("\n\"");
    PrintNStrSymbols(LineWithErr, lineSize);
    printf("\"\n");
    PrintNSpaces(errLinePos);
    COLOR_PRINT(RED, "^\n");

    COLOR_PRINT(WHITE, "%s\n", msg);

    COLOR_PRINT(VIOLET, "\nexit() in 3, 2, 1...\n");
    // exit(-28);
    abort();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* FindNline(const char* str, size_t nLine, size_t* lineSize)
{
    assert(str);
    assert(lineSize);

    nLine--;

    size_t lp = 0;  // line pointer
    size_t sp = 0;  // str  pointer

    for (lp = 0; lp < nLine && str[lp] != '\0'; sp++)
    {
        if (str[sp] == '\n') lp++;
    }

    assert(str[lp] != '\0' && "str doesn't have stolko lines\n");

    size_t old_sp = sp;

    while (str[sp] != '\n' && str[sp] != '\0') sp++;

    *lineSize = sp - old_sp;

    return str + old_sp;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintNStrSymbols(const char* str, size_t n)
{
    assert(str);

    for (size_t i = 0; i < n; i++)
    {
        printf("%c", str[i]);
    }

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintNSpaces(size_t nSpace)
{
    for (size_t i = 0; i < nSpace; i++)
    {
        printf(" ");
    }

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
