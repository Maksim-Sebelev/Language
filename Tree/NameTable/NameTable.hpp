#ifndef NAME_TABLE_HPP
#define NAME_TABLE_HPP

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "../../Common/GlobalInclude.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum NameTableErrorType
{
    NO_ERR,
    CTOR_CALLOC_NULL,
    TABLE_NULLPTR,
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct NameTableErr
{
    CodePlace          place;
    NameTableErrorType err;
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Variable
{
    char*  name;
    size_t nameLen;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct NameTable
{
    Variable* table;
    size_t    size;
    size_t    capacity;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void NameTableAssertPrint(const NameTableErr* err, const char* file, const int line, const char* func);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NAME_TABLE_ASSERT(err) do                                       \
{                                                                        \
    NameTableErr errCopy = err;                                           \
    if (errCopy.err != NameTableErrorType::NO_ERR)                         \
    {                                                                       \
        NameTableAssertPrint(err, __FILE__, __LINE__, __func__);             \
        exit(-1);                                                             \
    }                                                                          \
} while (0)                                                                     \


#endif