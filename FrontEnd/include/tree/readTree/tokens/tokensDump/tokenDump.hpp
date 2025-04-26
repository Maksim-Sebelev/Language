#ifndef TOKEN_DUMP_H
#define TOKEN_DUMP_H

#include <stddef.h>
#include "tree/readTree/tokens/token.hpp"

void TokenTextDump    (const Token_t* tokenArr, size_t tokenNum, const char* file, const int line, const char* func);
void TokenGraphicDump (const Token_t* tokenArr, size_t arrSize, const char* file, const int line, const char* func);


#define TOKEN_GRAPHIC_DUMP(tokenArr, arrSize)  TokenGraphicDump(tokenArr, arrSize,  __FILE__, __LINE__, __func__)
#define TOKEN_TEXT_DUMP(   token,    tokenNum) TokenTextDump   (token,    tokenNum, __FILE__, __LINE__, __func__)


#endif
