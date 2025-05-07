#include <assert.h>
#include "log/log.hpp"
#include "name-table/name-table-log/name-table-log.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void LogNameName(Name name);
static const char* GetNameTypeInStr(Name name);
static void LogNameWithPointer(Name name, size_t pointer);


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogName(Name name)
{
    LogPrint    (Yellow, "name = \n{\n"                          );

    LogPrint    (Green,  "name = "); LogNameName(name            );
    LogTextColor(Green                                           );
    LogAdcPrint (        "type = '%s'"   , GetNameTypeInStr(name));
    // LogAdcPrint (        "id   = '%lu'\n", name.id               );

    LogPrint    (Yellow, "}\n\n"                                 );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogNameTable(const NameTable_t* table)
{
    assert(table);

    size_t size = table->size;

    LogPrint(Red, "Name table size = '%lu'\n\n", size);

    for (size_t i = 0; i < size; i++)
    {
        LogNameWithPointer(table->data[i], i);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void LogNameWithPointer(Name name, size_t pointer)
{
    LogPrint    (Yellow, "name[%lu] = \n{\n", pointer            );

    LogPrint    (Green,  "name = "); LogNameName(name            );
    LogTextColor(Green                                           );
    LogAdcPrint (        "type = '%s'"   , GetNameTypeInStr(name));
    // LogAdcPrint (        "id   = '%lu'\n", name.id               );

    LogPrint    (Yellow, "}\n\n"                                 );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void LogNameName(Name name)
{
    const char* nameName = name.name.name;
    size_t      nameLen  = name.name.len;

    LogPrint(Red, "'");
    LogTextColor(Green);

    for (size_t i = 0; i < nameLen; i++)
    {
        LogAdcPrint("%c", nameName[i]);
    }

    LogPrint(Red, "'\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetNameTypeInStr(Name name)
{
    NameType type = name.type;

    switch (type)
    {
        case NameType::variable: return "variable";
        case NameType::function: return "function";
        case NameType::undefined_name_type:
        default: assert(0 && "you forgot about some name type");
    }

    assert(0 && "wtf?");
    return "undefined";
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
