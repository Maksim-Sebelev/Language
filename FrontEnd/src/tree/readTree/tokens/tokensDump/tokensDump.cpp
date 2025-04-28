#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "tree/treeDump/globalDump.hpp"
#include "tree/readTree/tokens/token.hpp"
#include "tree/readTree/tokens/tokensDump/tokenDump.hpp"


static void TokenGraphicDumpHelper(const Token_t* tokenArr, size_t arrSize, const char* dotFileName, const char* file, const int line, const char* func);

static void DotTokenBegin    (FILE* dotFile);
static void CreateAllTokens  (const Token_t* tokenArr, size_t arrSize, FILE* dotFile);
static void CreateToken      (const Token_t* token,    size_t pointer, FILE* dotFile);

static const char* GetTokenColor     (const Token_t* token);
static const char* GetTokenTypeInStr (const Token_t* token);
static const char* GetTokenDataInStr (const Token_t* token);



//=============================== Token Dump =============================================================================================================================================

void TokenTextDump(const Token_t* tokenArr, size_t tokenNum, const char* file, const int line, const char* func)
{
    // assert(tokenArr);
    // assert(file);
    // assert(file);

    // COLOR_PRINT(GREEN, "\nToken Dump:\n\n");

    // PRINT_PLACE(BLUE, file, line, func);

    // COLOR_PRINT(GREEN, "\n\ntoken[%lu]:\n", tokenNum);

    // COLOR_PRINT(CYAN, "type: '%s'\n", GetTokenTypeInStr(&tokenArr[tokenNum]));

    // if (tokenArr[tokenNum].type == TokenType::TokenNumber_t)
    // {
    //     Number number = tokenArr[tokenNum].data.number;
    //     switch (number.type)
    //     {
    //         case Type::int_t: COLOR_PRINT(RED, "vae")
    //     }
    // }

    // else
    // {
    //     COLOR_PRINT(CYAN, "data: '%s'\n", GetTokenDataInStr(&tokenArr[tokenNum]));
    // }

    // COLOR_PRINT(GREEN, "\nToken Dump End.\n\n\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokenGraphicDump(const Token_t* tokenArr, size_t arrSize, const char* file, const int line, const char* func)
{
    assert(tokenArr);
    assert(file);
    assert(func);

    system("rm -rf dot/tokens/img/*");
    system("rm -rf dot/tokens/img/*");


    system("mkdir -p dot/");
    system("mkdir -p dot/tokens/");
    system("mkdir -p dot/tokens/img/");
    system("mkdir -p dot/tokens/dot/");

    static size_t ImgQuant = 1;

    static const size_t MaxfileNameLen = 128;
    char outfile[MaxfileNameLen] = {};
    sprintf(outfile, "dot/tokens/img/tokens%lu.png", ImgQuant);
    
    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    
    char dotFileName[MaxfileNameLen] = {};
    sprintf(dotFileName, "dot/tokens/dot/tokens%lu.dot", ImgQuant);
    sprintf(command, "dot -Tpng %s > %s", dotFileName, outfile);
    
    TokenGraphicDumpHelper(tokenArr, arrSize, dotFileName, file, line, func);
    system(command);
    
    ImgQuant++;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void TokenGraphicDumpHelper(const Token_t* tokenArr, size_t arrSize, const char* dotFileName, const char* file, const int line, const char* func)
{
    assert(tokenArr);
    assert(file);
    assert(func);

    FILE* dotFile = fopen(dotFileName, "w");
    assert(dotFile);

    DotTokenBegin(dotFile);

    DotCreateDumpPlace(dotFile, file, line, func);

    CreateAllTokens(tokenArr, arrSize, dotFile);
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

static void CreateAllTokens(const Token_t* tokenArr, size_t arrSize, FILE* dotFile)
{
    assert(tokenArr);
    assert(dotFile);

    for (size_t i = 0; i < arrSize; i++)
    {
        CreateToken(&tokenArr[i], i, dotFile);
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
        case TokenType::TokenFunction_t:    return "#0cf108";
        case TokenType::TokenBracket_t:     return "#e69c0c";
        case TokenType::TokenSeparator_t:   return "#fdc500";
        case TokenType::TokenEndSymbol_t:   return "#ffffff";
        case TokenType::TokenType_t:        return "#CD5C5C";
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
        case TokenType::TokenNumber_t:    return "number";
        case TokenType::TokenType_t:      return "type";
        case TokenType::TokenOperation_t: return "operation";
        case TokenType::TokenFunction_t:  return "function";
        case TokenType::TokenBracket_t:   return "bracket";
        case TokenType::TokenSeparator_t: return "separator";
        case TokenType::TokenEndSymbol_t: return "end";
        case TokenType::TokenName_t:      return "name";
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
            Type type = token->data.type;
            return GetTypeInStr(type);
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
            EndSymbol end = token->data.end;
            return "\\0";
        }
        default: assert(0 && "undefined type."); return "undefined";
    }

    return "undefined";
}

//=============================== Token Dump End =============================================================================================================================================