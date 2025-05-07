#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "dump/global-dump.hpp"
#include "read-tree/tokens/tokens.hpp"
#include "read-tree/tokens/tokens-dump/tokens-dump.hpp"
#include "log/log.hpp"



static void TokenGraphicDumpHelper(const TokensArr* tokensArr, const char* dotFileName, const char* file, const int line, const char* func);

static void DotTokenBegin    (FILE* dotFile);
static void CreateAllTokens  (const TokensArr* tokensArr, FILE* dotFile);
static void CreateToken      (const Token_t* token,    size_t pointer, FILE* dotFile);

static const char* GetTokenColor     (const Token_t* token);
static const char* GetTokenTypeInStr (const Token_t* token);
static const char* GetTokenDataInStr (const Token_t* token);



//=============================== Token Dump =============================================================================================================================================

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokensLog(const TokensArr* tokensArr, const InputData* inputData)
{
    assert(tokensArr);
    assert(inputData);

    LOG_PRINT(Red, "Tokens Dump\n");

    
    LOG_PRINT(White, "log made in:\n");
    LOG_PLACE(White);


    size_t size = tokensArr->size;
    Token_t* arr = tokensArr->arr;

    for (size_t i = 0; i < size; i++)
    {
        LOG_PRINT(Green, "token[%lu] = \n{\n", i);
        TokenLog(arr + i, inputData);
        LOG_PRINT(Green, "}\n\n");
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokenLog(const Token_t* token, const InputData* input)
{
    assert(token);
    assert(input);

    TokenType token_type = token->type;
    FilePlace place      = token->place;
    
    LOG_PRINT(White, "%s:%lu:%lu\n", input->inputStream, place.line, place.placeInLine);
    
    const char* token_type_str = GetTokenTypeInStr(token);

    LOG_PRINT(Blue, "type = '%s'\n", token_type_str);


    LOG_COLOR(Red);

    if (token_type == TokenType::TokenNumber_t)
    {
        Type        token_num_type  = token->data.number.type;
        NumberValue token_num_value = token->data.number.value;
        switch (token_num_type)
        {
            case Type::int_type:    LOG_ADC_PRINT("value = '%d'", token_num_value.int_val   ); break;
            case Type::double_type: LOG_ADC_PRINT("value = '%f'", token_num_value.double_val); break;
            case Type::char_type:   LOG_ADC_PRINT("value = '%c'", token_num_value.char_val  ); break;
            case Type::void_type:
            case Type::undefined_type:
            default:                assert(0 && "something went wrong");
        }
    }

    else if (token_type == TokenType::TokenName_t)
    {
        Name        name      = token->data.name;
        size_t      name_len  = name.name.len;
        const char* name_name = name.name.name;

        char buffer[50] = {};
        snprintf(buffer, name_len + 1, "%s", name_name);
        LOG_ADC_PRINT("value = '%s'", buffer);
    }

    else
    {
        LOG_ADC_PRINT("value = '%s'", GetTokenDataInStr(token));
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokenGraphicDump(const TokensArr* tokensArr, const char* file, const int line, const char* func)
{
    assert(tokensArr);
    assert(file);
    assert(func);

    system("mkdir -p ../dot/");
    system("mkdir -p ../dot/tokens/");
    system("mkdir -p ../dot/tokens/img/");
    system("mkdir -p ../dot/tokens/dot/");

    static size_t ImgQuant = 1;

    static const size_t MaxBufferLen = 128;
    char outfile[MaxBufferLen] = {};
    snprintf(outfile, MaxBufferLen, "../dot/tokens/img/tokens%lu.png", ImgQuant);
    
    
    char dotFileName[MaxBufferLen] = {};
    snprintf(dotFileName, MaxBufferLen, "../dot/tokens/dot/tokens%lu.dot", ImgQuant);
    
    TokenGraphicDumpHelper(tokensArr, dotFileName, file, line, func);


    static const size_t MaxCommandLen = 512;
    char command[MaxCommandLen] = {};
    snprintf(command, MaxCommandLen, "dot -Tpng %s > %s", dotFileName, outfile);
    system(command);
    
    ImgQuant++;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void TokenGraphicDumpHelper(const TokensArr* tokensArr, const char* dotFileName, const char* file, const int line, const char* func)
{
    assert(tokensArr);
    assert(file);
    assert(func);

    FILE* dotFile = fopen(dotFileName, "w");
    assert(dotFile);

    DotTokenBegin(dotFile);

    DotCreateDumpPlace(dotFile, file, line, func);

    CreateAllTokens(tokensArr, dotFile);
    DotEnd(dotFile);

    fclose(dotFile);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotTokenBegin(FILE* dotFile)
{
    assert(dotFile);
    fprintf(dotFile, "digraph G{\nrankdir=TB\ngraph [bgcolor=\"#000000\"];\nsize = \"100 5\"\n");
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CreateAllTokens(const TokensArr* tokensArr, FILE* dotFile)
{
    assert(tokensArr);
    assert(dotFile);

    size_t size = tokensArr->size;
    Token_t* arr = tokensArr->arr;
    for (size_t i = 0; i < size; i++)
    {
        CreateToken(arr + i, i, dotFile);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CreateToken(const Token_t* token, size_t pointer, FILE* dotFile)
{
    assert(dotFile);

    if (!token)
    {
        fprintf(dotFile, "token%lu", pointer);
        fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#0125fe\", ");
        fprintf(dotFile, "label = \"nullptr\"");
        fprintf(dotFile, "color = \"#777777\"];\n");
        return;
    }

    const char* tokenColor = GetTokenColor(token);

    assert(tokenColor);

    TokenType type = token->type;

    fprintf(dotFile, "token%lu", pointer);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"%s\",", tokenColor);

    const char* tokenType = GetTokenTypeInStr(token);
    assert(tokenType);

    fprintf(dotFile, "label = \" { %s | ", tokenType);

    if (type == TokenType::TokenNumber_t)
    {
        Number number = token->data.number;
        FprintNumber(dotFile, number);
    }

    else if (type == TokenType::TokenName_t)
    {
        Name name = token->data.name;
        FprintName(dotFile, name);
    }

    else
    {
        const char* tokenData = GetTokenDataInStr(token);
        if (!isalpha(tokenData[0])) fprintf(dotFile, "\\");

        fprintf(dotFile, "%s", tokenData);

    }

    fprintf(dotFile, " | ");
    fprintf(dotFile, " token[%lu] | ", pointer);
    fprintf(dotFile, " input:%lu:%lu } \", ", token->place.line, token->place.placeInLine);
    fprintf(dotFile, "color = \"#777777\"];\n");

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetTokenColor(const Token_t* token)
{
    assert(token);

    TokenType type = token->type;

    switch (type)
    {
        case TokenType::TokenNumber_t:      return "#1cb9ff";
        case TokenType::TokenName_t:        return "#f31807";
        case TokenType::TokenOperation_t:   return "#00ca2c";
        case TokenType::TokenDefaultFunc_t: return "#0cf108";
        case TokenType::TokenBracket_t:     return "#e69c0c";
        case TokenType::TokenSeparator_t:   return "#fdc500";
        case TokenType::TokenEndSymbol_t:   return "#ffffff";
        case TokenType::TokenType_t:        return "#CD5C5C";
        case TokenType::TokenCondition_t:   return "#CDFC5C";
        case TokenType::TokenCycle_t:       return "#CFFC5C";
        case TokenType::TokenFuncAttr_t:    return "#01f0af";
        default: assert(0 && "undefined token type."); break; 
    }

    assert(0 && "we must not be here");
    return "wtf";
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetTokenTypeInStr(const Token_t* token)
{
    assert(token);

    TokenType type = token->type;

    switch (type)
    {
        case TokenType::TokenNumber_t:
        {
            Type number_type = token->data.number.type;
            switch (number_type)
            {
                case Type::int_type:    return "number/int";
                case Type::double_type: return "number/double";
                case Type::char_type:   return "number/char";
                case Type::void_type:
                case Type::undefined_type:
                default: assert(0 && "undefined num type"); return "number/undefined";
            }
        }
        case TokenType::TokenType_t:        return "type";
        case TokenType::TokenOperation_t:   return "operation";
        case TokenType::TokenDefaultFunc_t: return "function";
        case TokenType::TokenBracket_t:     return "bracket";
        case TokenType::TokenSeparator_t:   return "separator";
        case TokenType::TokenEndSymbol_t:   return "end";
        case TokenType::TokenName_t:        return "name";
        case TokenType::TokenCondition_t:   return "condition";
        case TokenType::TokenCycle_t:       return "cycle";
        case TokenType::TokenFuncAttr_t:    return "function attribute";
        default: assert(0 && "nudefindef type."); return "undefined";
    }

    return "undefined";
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetTokenDataInStr(const Token_t* token)
{
    assert(token);

    TokenType type = token->type;

    switch (type)
    {
        case TokenType::TokenNumber_t:
        {    
            assert(0 && "Number is drugaja situation.");
            return "undefined";
        }

        case TokenType::TokenName_t:
        {
            assert(0 && "Name is drugaja situation.");
            break;
        }
        
        case TokenType::TokenType_t:
        {
            Type typ = token->data.type;
            return GetTypeInStr(typ);
        }

        case TokenType::TokenOperation_t:
        {
            Operation operation = token->data.operation;
            return GetOperationInStr(operation); //
        }

        case TokenType::TokenSeparator_t:
        {
            Separator separator = token->data.separator;
            return GetSeparatorInStr(separator);
        }

        case TokenType::TokenBracket_t:
        {
            Bracket bracket = token->data.bracket;
            return GetBracketInStr(bracket);
        }
    
        case TokenType::TokenEndSymbol_t:
        {
            return "\\0";
        }

        case TokenType::TokenCondition_t:
        {
            Condition condition = token->data.condition;
            return GetConditionInStr(condition);
        }

        case TokenType::TokenCycle_t:
        {
            Cycle cycle = token->data.cycle;
            return GetCycleInStr(cycle);
        }

        case TokenType::TokenFuncAttr_t:
        {
            FunctionAttribute attribute = token->data.attribute;
            return GetFuncAttrInStr(attribute);
        }

        case TokenType::TokenDefaultFunc_t:
        {
            DFunction function = token->data.function;
            return GetFuncInStr(function);
        }

        default: assert(0 && "undefined type."); return "undefined";
    }

    return "undefined";
}

//=============================== Token Dump End =============================================================================================================================================
