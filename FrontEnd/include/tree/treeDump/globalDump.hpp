#ifndef GLOABL_DUMP_HPP
#define GLOBAL_DUMP_HPP

#include <stdio.h>
#include "tree/tree.hpp"

void         DotEnd             (FILE* dotFile);
void         DotCreateDumpPlace (FILE* dotFile, const char* file, const int line, const char* func);
const char*  GetTypeInStr        (Type type);
const char*  GetOperationInStr  (Operation oper);
const char*  GetFuncInStr       (DFunction func);
bool         IsDoubleEqual      (double firstNum, double secondNum, double epss);

#endif // GLOBAL_DUMP_HPP
