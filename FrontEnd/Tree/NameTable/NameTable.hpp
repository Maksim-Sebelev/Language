#ifndef NAME_TABLE_H
#define NAME_TABLE_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../Common/ColorPrint.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NAME_TABLE_DEBUG
#define NAME_TABLE_CANARY
#define NAME_TABLE_DATA_CANARY
#define NAME_TABLE_HASH
#define NAME_TABLE_DATA_HASH
// #define NAME_TABLE_DATA_POISON

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_DEBUG
    #define ON_NAME_TABLE_DEBUG(...) __VA_ARGS__
    #define OFF_NAME_TABLE_DEBUG(...)
#else
    #define ON_NAME_TABLE_DEBUG(...)
    #define OFF_NAME_TABLE_DEBUG(...) __VA_ARGS__
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_CANARY
    #define ON_NAME_TABLE_CANARY(...) __VA_ARGS__
#else
    #define ON_NAME_TABLE_CANARY(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_DATA_CANARY
    #define ON_NAME_TABLE_DATA_CANARY(...)  __VA_ARGS__
#else
    #define ON_NAME_TABLE_DATA_CANARY(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_HASH
    #define ON_NAME_TABLE_HASH(...) __VA_ARGS__
#else   
    #define ON_NAME_TABLE_HASH(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_DATA_HASH
    #define ON_NAME_TABLE_DATA_HASH(...) __VA_ARGS__
#else
    #define ON_NAME_TABLE_DATA_HASH(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_DATA_POISON
    #define ON_NAME_TABLE_DATA_POISON(...) __VA_ARGS__
#else
    #define ON_NAME_TABLE_DATA_POISON(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum class NameType
{
    Variable,
    Function,
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Name
{
    const char* name;
    size_t      nameLen;
    NameType    type;
    size_t      id;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_NAME_TABLE_CANARY(typedef uint64_t NameTableCanary_t;)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct NameTable_t
{
    ON_NAME_TABLE_CANARY(NameTableCanary_t leftNameTableCanary;)
    size_t size;
    size_t capacity;
    Name*  data;
    ON_NAME_TABLE_HASH(uint64_t nameTableHash;)
    ON_NAME_TABLE_DATA_HASH(uint64_t dataHash;)
    ON_NAME_TABLE_CANARY(NameTableCanary_t rightNameTableCanary;)
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Warnings
{
    unsigned char PopInEmptyNameTable             : 1;
    unsigned char TooBigCapacity                  : 1;
    unsigned char PushInFullNameTable             : 1;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct FatalErrors
{
    unsigned char NameTableNull                   : 1;
    unsigned char DataNull                    : 1;
    unsigned char CallocCtorNull              : 1;
    unsigned char ReallocPushNull             : 1;
    unsigned char ReallocPopNull              : 1;
    ON_NAME_TABLE_CANARY
    (
    unsigned char LeftNameTableCanaryChanged      : 1;
    unsigned char RightNameTableCanaryChanged     : 1;
    )
    ON_NAME_TABLE_DATA_CANARY
    (
    unsigned char LeftDataCanaryChanged       : 1;
    unsigned char RightDataCanaryChanged      : 1;
    )
    ON_NAME_TABLE_DATA_POISON
    (
    unsigned char DataElemBiggerSizeNotPoison : 1;
    )
    ON_NAME_TABLE_HASH
    (    
    unsigned char NameTableHashChanged            : 1;
    )
    ON_NAME_TABLE_DATA_HASH
    (
    unsigned char DataHashChanged             : 1;
    )
    ON_NAME_TABLE_DEBUG
    (
    unsigned char SizeBiggerCapacity          : 1;
    unsigned char CapacitySmallerMin          : 1;
    unsigned char CapacityBiggerMax           : 1;
    unsigned char CtorNameTableNameNull           : 1;
    unsigned char CtorNameTableFileNull           : 1;
    unsigned char CtorNameTableFuncNull           : 1;
    unsigned char CtorNameTableLineNegative       : 1;
    )
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct NameTabelErrorType
{
    unsigned int IsFatalError : 1;
    unsigned int IsWarning    : 1;
    Warnings     Warning;
    FatalErrors  FatalError;
    const char*  file;
    int          line;
    const char*  func;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NameTabelErrorType NameTableCtor               (NameTable_t* nameTable, size_t NameTableDataSize);
NameTabelErrorType NameTableDtor               (NameTable_t* nameTable);
NameTabelErrorType PrintNameTable              (NameTable_t* nameTable);
NameTabelErrorType PrintLastNameTableElem      (NameTable_t* nameTable);
NameTabelErrorType NameTablePush               (NameTable_t* nameTable, Name PushElem);
NameTabelErrorType NameTablePop                (NameTable_t* nameTable, Name* PopElem);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AssertPrint (NameTabelErrorType Err, const char* file, int line, const char* func);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef NAME_TABLE_DEBUG
    #define NAME_TABLE_ASSERT(Err) do                             \
    {                                                         \
        NameTabelErrorType ErrCopy = Err;                               \
        if (ErrCopy.IsFatalError || ErrCopy.IsWarning)          \
        {                                                        \
            AssertPrint(ErrCopy, __FILE__, __LINE__, __func__);   \
            COLOR_PRINT(CYAN, "abort() in 3, 2, 1...\n");          \
            abort();                                                \
        }                                                            \
    } while (0)                                                       \

#else
    #define NAME_TABLE_ASSERT(Err) AssertPrint(Err, __FILE__, __LINE__, __func__)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif
