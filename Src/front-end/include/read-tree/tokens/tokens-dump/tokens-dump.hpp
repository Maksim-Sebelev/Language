#ifndef TOKENS_DUMP_HPP
#define TOKENS_DUMP_HPP

#include <stddef.h>
#include "read-tree/tokens/tokens.hpp"

void TokensLog        (const TokensArr* tokensArr, const InputData* inputData);
void TokenLog         (const Token_t* token, const InputData* inputData);
void TokenGraphicDump (const TokensArr* tokensArr, const char* file, const int line, const char* func);


#define TOKEN_GRAPHIC_DUMP(tokenArr                )  TokenGraphicDump(tokenArr,  __FILE__, __LINE__, __func__)
#define TOKENS_LOG(        tokenArr, arrSize, input)  TokensLog       (tokenArr, arrSize, input                          )
#define TOKEN_LOG(         tokenArr,          input)  TokenLog        (token, input                                      )


#endif // TOKENS_DUMP_HPP
