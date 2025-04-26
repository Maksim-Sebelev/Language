#ifndef NAMETABLE_DUMP_HPP
#define NAMETABLE_DUMP_HPP


#include "../NameTable.hpp"

void NameTableDump(const NameTable_t* table, const char* file, const int line, const char* func);

#define NAME_TABLE_GRAPHIC_DUMP(table) NameTableDump(table, __FILE__, __LINE__, __func__)

#endif
