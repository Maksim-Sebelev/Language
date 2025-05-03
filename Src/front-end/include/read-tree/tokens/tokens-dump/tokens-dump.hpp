#ifndef TOKEN_DUMP_H
#define TOKEN_DUMP_H

#include <stddef.h>
#include "tree/readTree/tokens/token.hpp"

void TokensLog        (const Token_t* tokens, size_t tokensQuant, const InputData* inputData);
void TokenLog         (const Token_t* token, const InputData* inputData);
void TokenGraphicDump (const Token_t* tokens, size_t tokensQuant, const char* file, const int line, const char* func);


#define TOKEN_GRAPHIC_DUMP(tokenArr, arrSize)         TokenGraphicDump(tokenArr, arrSize,  __FILE__, __LINE__, __func__)
#define TOKENS_LOG(        tokenArr, arrSize, input)  TokensLog       (tokenArr, arrSize, input                        )
#define TOKEN_LOG(         tokenArr,          input)  TokenLog        (token, input                                    )


#endif
