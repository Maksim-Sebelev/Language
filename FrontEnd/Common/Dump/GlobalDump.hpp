#ifndef GLOABL_DUMP_H
#define GLOBAL_DUMP_H

#include <stdio.h>
#include "../../Tree/Tree.hpp"

void         DotEnd             (FILE* dotFile);
void         DotCreateDumpPlace (FILE* dotFile, const char* file, const int line, const char* func);
const char*  GetOperationInStr  (Operation oper);
const char*  GetFuncInStr       (Function func);
bool         IsDoubleEqual      (double firstNum, double secondNum, double epss);

#endif
