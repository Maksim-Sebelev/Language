#ifndef TOKEN_DUMP_H
#define TOKEN_DUMP_H

#include <stddef.h>
#include "tree/readTree/tokens/token.hpp"

void TokensLog        (const Token_t* tokens, size_t tokensQuant, const InputData* inputData);
void TokenGraphicDump (const Token_t* tokens, size_t tokensQuant, const char* file, const int line, const char* func);


#define TOKEN_GRAPHIC_DUMP(tokenArr, arrSize)        TokenGraphicDump(tokenArr, arrSize,  __FILE__, __LINE__, __func__)
#define TOKEN_LOG(         tokenArr, arrSize, input) TokensLog       (tokenArr, arrSize, input                        )


#endif
