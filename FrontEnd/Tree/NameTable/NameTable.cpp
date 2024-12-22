#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include "NameTable.hpp"
#include "Hash.hpp"
#include "../../Common/ColorPrint.hpp"

static const size_t MinCapacity = 1<<3;
static const size_t MaxCapacity = 1<<21;

static const unsigned int CapPushReallocCoef = 2;
static const unsigned int CapPopReallocCoef  = 4;

ON_NAME_TABLE_DATA_CANARY
(
typedef uint64_t DataCanary_t;
)

ON_NAME_TABLE_DATA_CANARY
(
static const NameTableCanary_t leftNameTableCanary  = 0xDEEADDEADDEAD;
static const NameTableCanary_t rightNameTableCanary = 0xDEADDEDDEADED;
)
ON_NAME_TABLE_DATA_CANARY
(
static const DataCanary_t LeftDataCanary    = 0xEDADEDAEDADEDA;
static const DataCanary_t RightDataCanary   = 0xDEDDEADDEDDEAD;
)
ON_NAME_TABLE_HASH
(
static const uint64_t DefaultNameTableHash = 538176576;
)
ON_NAME_TABLE_DATA_POISON
(
static const Name Poison = {"", 0, 0};
)

static size_t GetNewCapacity      (size_t capacity); 
static size_t GetNewCtorCapacity  (size_t NameTableDataSize);
static size_t GetNewPushCapacity  (const NameTable_t* nameTable);
static size_t GetNewPopCapacity   (const NameTable_t* nameTable);

static NameTabelErrorType CtorCalloc   (NameTable_t* nameTable);
static NameTabelErrorType DtorFreeData (NameTable_t* nameTable);
static NameTabelErrorType PushRealloc  (NameTable_t* nameTable);
static NameTabelErrorType PopRealloc   (NameTable_t* nameTable);

ON_NAME_TABLE_DATA_CANARY
(
static DataCanary_t GetLeftDataCanary    (const NameTable_t* nameTable);
static DataCanary_t GetRightDataCanary   (const NameTable_t* nameTable);
static void         SetLeftDataCanary    (NameTable_t* nameTable);
static void         SetRightDataCanary   (NameTable_t* nameTable);
static NameTabelErrorType    MoveDataToLeftCanary (NameTable_t* nameTable);
static NameTabelErrorType    MoveDataToFirstElem  (NameTable_t* nameTable);
)

ON_NAME_TABLE_DATA_HASH
(
static uint64_t CalcDataHash(const NameTable_t* nameTable);
)
ON_NAME_TABLE_HASH
(
static uint64_t CalcNameTableHash (NameTable_t* nameTable);
)

static NameTabelErrorType Verif       (NameTable_t* nameTable, NameTabelErrorType* Error ON_NAME_TABLE_DEBUG(, const char* file, int line, const char* func));
static void      PrintError  (NameTabelErrorType Error);
static void      PrintPlace  (const char* file, int line, const char* Function);
ON_NAME_TABLE_DEBUG
(
static void      ErrPlaceCtor (NameTabelErrorType* err, const char* file, int line, const char* func);
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NAME_TABLE_VERIF(NameTablePtr, err) Verif(NameTablePtr, &err ON_NAME_TABLE_DEBUG(, __FILE__, __LINE__, __func__))

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RETURN_IF_ERR_OR_WARN(NameTablePtr, err) do                             \
{                                                                                \
    NameTabelErrorType ErrCopy = err;                                             \
    Verif(nameTable, &ErrCopy ON_NAME_TABLE_DEBUG(, __FILE__, __LINE__, __func__));     \
    if (ErrCopy.IsFatalError == 1 || ErrCopy.IsWarning == 1)                        \
    {                                                                                \
        return ErrCopy;                                                               \
    }                                                                                  \
} while (0)                                                                             \

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NameTabelErrorType NameTableCtor(NameTable_t* nameTable, size_t NameTableDataSize)
{
    NameTabelErrorType err = {};

    nameTable->size = 0;

    nameTable->capacity = GetNewCtorCapacity(NameTableDataSize);
    NAME_TABLE_ASSERT(CtorCalloc(nameTable));

    ON_NAME_TABLE_DATA_CANARY
    (
    nameTable->leftNameTableCanary  = leftNameTableCanary;
    nameTable->rightNameTableCanary = rightNameTableCanary;
    )

    ON_NAME_TABLE_DATA_CANARY
    (
    SetLeftDataCanary (nameTable);
    SetRightDataCanary(nameTable);
    )

    ON_NAME_TABLE_DATA_POISON
    (
    for (size_t data_i = 0; data_i < nameTable->capacity; data_i++)
    {
        nameTable->data[data_i] = Poison;
    }
    )

    ON_NAME_TABLE_DATA_HASH(nameTable->dataHash  = CalcDataHash(nameTable);)
    ON_NAME_TABLE_HASH(nameTable->nameTableHash = CalcNameTableHash(nameTable);)

    return NAME_TABLE_VERIF(nameTable, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NameTabelErrorType NameTableDtor(NameTable_t* nameTable)
{
    assert(nameTable);
    NameTabelErrorType err   = {};
    NAME_TABLE_ASSERT(DtorFreeData(nameTable));
    nameTable->data     = nullptr;
    nameTable->capacity = 0;
    nameTable->size     = 0;
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NameTabelErrorType NameTablePush(NameTable_t*  nameTable, Name PushElem)
{
    NameTabelErrorType err = {};
    RETURN_IF_ERR_OR_WARN(nameTable, err);

    if (nameTable->size + 1 > MaxCapacity)
    {
        err.Warning.PushInFullNameTable = 1;
        err.IsWarning = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }

    nameTable->size++;

    if (nameTable->size <= nameTable->capacity)
    {
        nameTable->data[nameTable->size - 1] = PushElem;
    
        ON_NAME_TABLE_DATA_HASH(nameTable->dataHash  = CalcDataHash(nameTable);)
        ON_NAME_TABLE_HASH(nameTable->nameTableHash = CalcNameTableHash(nameTable);)

        return NAME_TABLE_VERIF(nameTable, err);
    }

    nameTable->capacity = GetNewPushCapacity(nameTable);
    NAME_TABLE_ASSERT(PushRealloc(nameTable));

    if (err.IsFatalError == 1)
    {
        return NAME_TABLE_VERIF(nameTable, err);
    }

    nameTable->data[nameTable->size - 1] = PushElem;

    ON_NAME_TABLE_DATA_CANARY(SetRightDataCanary(nameTable);)

    ON_NAME_TABLE_DATA_POISON
    (
    for (size_t data_i = nameTable->size; data_i < nameTable->capacity; data_i++)
    {
        nameTable->data[data_i] = Poison;
    }
    )

    ON_NAME_TABLE_DATA_HASH(nameTable->dataHash  = CalcDataHash(nameTable);)
    ON_NAME_TABLE_HASH(nameTable->nameTableHash = CalcNameTableHash(nameTable);)
    
    if (nameTable->capacity == MaxCapacity)
    {
        err.Warning.TooBigCapacity = 1;
        err.IsWarning = 1;
    }
    return NAME_TABLE_VERIF(nameTable, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NameTabelErrorType NameTablePop(NameTable_t* nameTable, Name* PopElem)
{
    NameTabelErrorType err = {};
    RETURN_IF_ERR_OR_WARN(nameTable, err);

    if (nameTable->size == 0)
    {
        err.Warning.PopInEmptyNameTable = 1;
        err.IsWarning = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }

    nameTable->size--;

    *PopElem = nameTable->data[nameTable->size];

    ON_NAME_TABLE_DATA_POISON(nameTable->data[nameTable->size] = Poison;)
    ON_NAME_TABLE_DATA_HASH(nameTable->dataHash  = CalcDataHash(nameTable);)
    ON_NAME_TABLE_HASH(nameTable->nameTableHash = CalcNameTableHash(nameTable);)

    if (nameTable->size * CapPopReallocCoef > nameTable->capacity)
    {
        return NAME_TABLE_VERIF(nameTable, err);
    }

    nameTable->capacity = GetNewPopCapacity(nameTable);
    NAME_TABLE_ASSERT(PopRealloc(nameTable));

    if (err.IsFatalError == 1)
    {
        return NAME_TABLE_VERIF(nameTable, err);
    }

    ON_NAME_TABLE_DATA_CANARY(SetRightDataCanary(nameTable);)
    ON_NAME_TABLE_DATA_HASH(nameTable->dataHash  = CalcDataHash(nameTable);)
    ON_NAME_TABLE_HASH(nameTable->nameTableHash = CalcNameTableHash(nameTable);)

    return NAME_TABLE_VERIF(nameTable, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_NAME_TABLE_HASH
(
static uint64_t CalcNameTableHash(NameTable_t* nameTable)
{
    assert(nameTable);

    const uint64_t NameTableHashCopy = nameTable->nameTableHash;
    nameTable->nameTableHash             = DefaultNameTableHash;
    uint64_t NewNameTableHash        = Hash(nameTable, 1, sizeof(NameTable_t));
    nameTable->nameTableHash             = NameTableHashCopy;
    return NewNameTableHash;
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_NAME_TABLE_DATA_HASH
(
static uint64_t CalcDataHash(const NameTable_t* nameTable)
{
    assert(nameTable);

    return Hash(nameTable->data, nameTable->capacity, sizeof(Name));
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_NAME_TABLE_DATA_CANARY
(
static DataCanary_t GetLeftDataCanary(const  NameTable_t* nameTable)
{
    assert(nameTable);

    return *(DataCanary_t*)((char*)nameTable->data - 1 * sizeof(DataCanary_t));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static DataCanary_t GetRightDataCanary(const NameTable_t* nameTable)
{
    assert(nameTable);

    return *(DataCanary_t*)((char*)nameTable->data + nameTable->capacity * sizeof(Name));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void SetLeftDataCanary(NameTable_t* nameTable)
{
    assert(nameTable);

    *(DataCanary_t*)((char*)nameTable->data - 1 * sizeof(DataCanary_t)) = LeftDataCanary;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void SetRightDataCanary(NameTable_t* nameTable)
{
    assert(nameTable);

    *(DataCanary_t*)((char*)nameTable->data + nameTable->capacity * sizeof(Name)) = RightDataCanary;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType MoveDataToLeftCanary(NameTable_t* nameTable)
{
    assert(nameTable);

    NameTabelErrorType err = {};

    nameTable->data = (Name*)((char*)nameTable->data - sizeof(LeftDataCanary) * sizeof(char));

    if (nameTable->data == nullptr)
    {
        err.FatalError.DataNull = 1;
        err.IsFatalError = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }

    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType MoveDataToFirstElem(NameTable_t* nameTable)
{
    assert(nameTable);

    NameTabelErrorType err = {};

    nameTable->data = (Name*)((char*)nameTable->data + sizeof(RightDataCanary) * sizeof(char));

    if (nameTable->data == nullptr)
    {
        err.FatalError.DataNull = 1;
        err.IsFatalError = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }

    return err;
}
)
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewCtorCapacity(const size_t NameTableDataSize)
{
    size_t Temp = (NameTableDataSize > MinCapacity) ? NameTableDataSize : MinCapacity;
    return GetNewCapacity(Temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPushCapacity(const NameTable_t* nameTable)
{
    assert(nameTable);

    size_t NewCapacity = (nameTable->capacity * CapPushReallocCoef);
    size_t Temp = NewCapacity < MaxCapacity ? NewCapacity : MaxCapacity;
    return GetNewCapacity(Temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPopCapacity(const NameTable_t* nameTable)
{
    assert(nameTable);

    size_t NewCapacity = (nameTable->capacity / CapPopReallocCoef);
    size_t Temp = (NewCapacity > MinCapacity) ? NewCapacity : MinCapacity;
    return GetNewCapacity(Temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewCapacity(size_t capacity)
{
    ON_NAME_TABLE_DATA_CANARY
    (
    size_t DataCanarySize = sizeof(DataCanary_t);
    size_t Temp = (capacity % DataCanarySize == 0) ? 0 : DataCanarySize - capacity % DataCanarySize;
    capacity += Temp;
    )
    return capacity;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType PushRealloc(NameTable_t* nameTable)
{
    assert(nameTable);
    NameTabelErrorType err = {};

    ON_NAME_TABLE_DATA_CANARY
    (
    NAME_TABLE_ASSERT(MoveDataToLeftCanary(nameTable));
    )
    nameTable->data = (Name*) realloc(nameTable->data, nameTable->capacity * sizeof(Name) ON_NAME_TABLE_DATA_CANARY(+ 2 * sizeof(DataCanary_t)));
    if (nameTable->data == nullptr)
    {
        err.FatalError.ReallocPushNull = 1;
        err.IsFatalError = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }
    ON_NAME_TABLE_DATA_CANARY
    (
    NAME_TABLE_ASSERT(MoveDataToFirstElem(nameTable));
    )
    return err; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType PopRealloc(NameTable_t* nameTable)
{
    assert(nameTable);

    NameTabelErrorType err = {};

    ON_NAME_TABLE_DATA_CANARY
    (
    NAME_TABLE_ASSERT(MoveDataToLeftCanary(nameTable));
    )
    nameTable->data = (Name*) realloc(nameTable->data, nameTable->capacity * sizeof(Name) ON_NAME_TABLE_DATA_CANARY(+ 2 * sizeof(DataCanary_t)));
    if (nameTable->data == nullptr)
    {
        err.FatalError.ReallocPopNull = 1;
        err.IsFatalError = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }
    ON_NAME_TABLE_DATA_CANARY
    (
    NAME_TABLE_ASSERT(MoveDataToFirstElem(nameTable));
    )
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType CtorCalloc(NameTable_t* nameTable)
{
    NameTabelErrorType err = {};
    assert(nameTable);

    nameTable->data = (Name*) calloc (nameTable->capacity * sizeof(Name) ON_NAME_TABLE_DATA_CANARY(+ 2 * sizeof(DataCanary_t)), sizeof(char));

    if (nameTable->data == nullptr)
    {
        err.FatalError.DataNull = 1;
        err.IsFatalError = 1;
        return NAME_TABLE_VERIF(nameTable, err);
    }
    ON_NAME_TABLE_DATA_CANARY
    (
    NAME_TABLE_ASSERT(MoveDataToFirstElem(nameTable));
    )
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType DtorFreeData(NameTable_t* nameTable)
{
    NameTabelErrorType err = {};
    RETURN_IF_ERR_OR_WARN(nameTable, err);

    ON_NAME_TABLE_DATA_CANARY
    (
    NAME_TABLE_ASSERT(MoveDataToLeftCanary(nameTable));
    )
    free(nameTable->data);
    nameTable->data = nullptr;

    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static NameTabelErrorType Verif(NameTable_t* nameTable, NameTabelErrorType* Error ON_NAME_TABLE_DEBUG(, const char* file, int line, const char* func))
{
    assert(Error);

    ON_NAME_TABLE_DEBUG
    (
    ErrPlaceCtor(Error, file, line, func);
    )

    if (nameTable == nullptr)
    {
        Error->FatalError.NameTableNull = 1;
        Error->IsFatalError = 1;
        return *Error;
    }
    else
    {
        Error->FatalError.NameTableNull = 0;   
    }

    if (nameTable->data == nullptr)
    {
        Error->FatalError.DataNull = 1;
        Error->IsFatalError = 1;
        return *Error;
    }
    else
    {
        Error->FatalError.DataNull = 0;
    }

    ON_NAME_TABLE_DATA_CANARY
    (
    if (nameTable->leftNameTableCanary != leftNameTableCanary)
    {
        Error->FatalError.LeftNameTableCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.LeftNameTableCanaryChanged = 0;
    }

    if (nameTable->rightNameTableCanary != rightNameTableCanary)
    {
        Error->FatalError.RightNameTableCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.RightNameTableCanaryChanged = 0;
    }
    )

    ON_NAME_TABLE_DATA_CANARY
    (
    if (GetLeftDataCanary(nameTable) != LeftDataCanary)
    {
        Error->FatalError.LeftDataCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.LeftDataCanaryChanged = 0;
    }

    if (GetRightDataCanary(nameTable) != RightDataCanary)
    {
        Error->FatalError.RightDataCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.RightDataCanaryChanged = 0;
    }
    )

    ON_NAME_TABLE_DEBUG
    (
    if (nameTable->size > nameTable->capacity)
    {
        Error->FatalError.SizeBiggerCapacity = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.SizeBiggerCapacity = 0;   
    }

    if (nameTable->capacity < MinCapacity)
    {
        Error->FatalError.CapacitySmallerMin = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CapacitySmallerMin = 0;
    }

    if (nameTable->capacity > MaxCapacity)
    {
        Error->FatalError.CapacityBiggerMax = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CapacityBiggerMax = 0;
    }
    )

    ON_NAME_TABLE_DATA_POISON
    (
    bool WasNotPosion = false;
    for (size_t data_i = nameTable->size; data_i < nameTable->capacity; data_i++)
    {
        if (nameTable->data[data_i] != Poison)
        {
            Error->FatalError.DataElemBiggerSizeNotPoison = 1;
            Error->IsFatalError = 1;
            WasNotPosion = true;
            break;
        }
    }

    if (!WasNotPosion)
    {
        Error->FatalError.DataElemBiggerSizeNotPoison = 0;
    }
    )

    ON_NAME_TABLE_DATA_HASH
    (
    if (CalcDataHash(nameTable) != nameTable->dataHash)
    {
        Error->FatalError.DataHashChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.DataHashChanged = 0;
    }
    )

    ON_NAME_TABLE_HASH
    (
    if (CalcNameTableHash(nameTable) != nameTable->nameTableHash)
    {
        Error->FatalError.NameTableHashChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.NameTableHashChanged = 0;
    }
    )
    return *Error;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintError(NameTabelErrorType Error)
{
    if (Error.IsWarning == 0 && Error.IsFatalError == 0)
    {
        return;
    }
    
    if (Error.IsWarning == 1)
    {
        if (Error.Warning.PopInEmptyNameTable == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: make pop, but nameTable is empty.\n");
            COLOR_PRINT(YELLOW, "NameTablePop will not change PopElem.\n");
        }

        if (Error.Warning.TooBigCapacity == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: to big data size.\n");
            COLOR_PRINT(YELLOW, "capacity have a max allowed value.\n");
        }

        if (Error.Warning.PushInFullNameTable == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: made NameTablePush in full nameTable.\n");
            COLOR_PRINT(YELLOW, "nameTable won't change.\n");
        }
    }

    if (Error.IsFatalError == 1)
    {
        if (Error.FatalError.NameTableNull == 1)
        {
            COLOR_PRINT(RED, "Error: Right data Canary was changed.\n");
            OFF_NAME_TABLE_DEBUG(COLOR_PRINT(RED, "!nameTable data can be incorrect!\n"));
        }

        if (Error.FatalError.DataNull == 1)
        {
            COLOR_PRINT(RED, "Error: data is nullptr.\n");
        }

        if (Error.FatalError.CallocCtorNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to allocate memory in NameTableCtor.\n");
        }

        if (Error.FatalError.ReallocPushNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to reallocate memory in NameTablePush.\n");
        }

        if (Error.FatalError.ReallocPopNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to free memory in NameTablePop.\n");
        }

        ON_NAME_TABLE_DATA_CANARY
        (
        if (Error.FatalError.LeftNameTableCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Left nameTable Canary was changed.\n");
            OFF_NAME_TABLE_DEBUG(COLOR_PRINT(RED, "!nameTable data can be incorrect!\n"));
        }
        
        if (Error.FatalError.RightNameTableCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Right nameTable Canary was changed.\n");
            OFF_NAME_TABLE_DEBUG(COLOR_PRINT(RED, "!nameTable data can be incorrect!\n"));
        }
        )
        ON_NAME_TABLE_DATA_CANARY
        (
        if (Error.FatalError.LeftDataCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Left data Canary was changed.\n");
            OFF_NAME_TABLE_DEBUG(COLOR_PRINT(RED, "!nameTable data can be incorrect!\n"));
        }

        if (Error.FatalError.RightDataCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Right data Canary was changed.\n");
            OFF_NAME_TABLE_DEBUG(COLOR_PRINT(RED, "!nameTable data can be incorrect!\n"));
        }
        )

        ON_NAME_TABLE_DATA_POISON
        (
        if (Error.FatalError.DataElemBiggerSizeNotPoison == 1)
        {
            COLOR_PRINT(RED, "Error: After size in data is not Poison elem.\n");
        }
        )

        ON_NAME_TABLE_DEBUG
        (
        if (Error.FatalError.SizeBiggerCapacity == 1)
        {
            COLOR_PRINT(RED, "Error: size > capacity.\n");
        }
        
        if (Error.FatalError.CapacityBiggerMax == 1)
        {
            COLOR_PRINT(RED, "Error: capacity > MaxCapacity.\n");
        }

        if (Error.FatalError.CapacitySmallerMin == 1)
        {
            COLOR_PRINT(RED, "Error: capacity < MinCapacity.\n");
        }

        if (Error.FatalError.CtorNameTableFileNull == 1)
        {
            COLOR_PRINT(RED, "Error: nameTable ctor init file is nullptr.\n");
        }

        if (Error.FatalError.CtorNameTableFuncNull == 1)
        {
            COLOR_PRINT(RED, "Error: nameTable ctor init func is nullptr.\n");
        }
        
        if (Error.FatalError.CtorNameTableLineNegative == 1)
        {
            COLOR_PRINT(RED, "Error: nameTable ctor init line is negative or 0.\n");
        }
        )

        ON_NAME_TABLE_DATA_HASH
        (
        if (Error.FatalError.DataHashChanged == 1)
        {
            COLOR_PRINT(RED, "Error: data Hash is incorrect.\n");
        }
        )

        ON_NAME_TABLE_HASH
        (
        if (Error.FatalError.NameTableHashChanged == 1)
        {
            COLOR_PRINT(RED, "Error: nameTable Hash is incorrect.\n");
        }
        )
    }
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_NAME_TABLE_DEBUG
(
static void ErrPlaceCtor (NameTabelErrorType* err, const char* file, int line, const char* func)
{
    assert(err);
    assert(file);
    assert(func);

    err->file = file;
    err->line = line;
    err->func = func;
    return;
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintPlace(const char* file, int line, const char* Function)
{
    COLOR_PRINT(WHITE, "file [%s]\nLine [%d]\nFunc [%s]\n", file, line, Function);
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AssertPrint(NameTabelErrorType err, const char* file, int line, const char* func)
{
    if (err.IsFatalError || err.IsWarning) 
    {
        COLOR_PRINT(RED, "Assert made in:\n");
        PrintPlace(file, line, func);
        PrintError(err);
        ON_NAME_TABLE_DEBUG
        (
        PrintPlace(err.file, err.line, err.func);
        )
        printf("\n");
    }
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef NAME_TABLE_VERIF
#undef RETURN_IF_ERR_OR_WARN
