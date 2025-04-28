#ifndef GLOABL_DUMP_HPP
#define GLOBAL_DUMP_HPP

#include <stdio.h>
#include "tree/tree.hpp"

void         DotEnd             (FILE* dotFile);
void         DotCreateDumpPlace (FILE* dotFile, const char* file, const int line, const char* func);

void         FprintNumber        (FILE* dotFile, Number number);
void         FprintName          (FILE* dotFile, Name   name);

const char*  GetTypeInStr       (Type type);
const char*  GetSeparatorInStr  (Separator separator);
const char*  GetOperationInStr  (Operation oper);
const char*  GetBracketInStr    (Bracket bracket);
const char*  GetCycleInStr      (Cycle cycle);
const char*  GetConditionInStr  (Condition condition);

bool         IsDoubleEqual      (double firstNum, double secondNum, double epss);

#endif // GLOBAL_DUMP_HPP
