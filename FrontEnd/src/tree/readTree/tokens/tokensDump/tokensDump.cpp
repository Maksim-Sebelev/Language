#include <stdio.h>
#include <assert.h>
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
    assert(tokenArr);
    assert(file);
    assert(file);

    COLOR_PRINT(GREEN, "\nToken Dump:\n\n");

    PRINT_PLACE(BLUE, file, line, func);

    COLOR_PRINT(GREEN, "\n\ntoken[%lu]:\n", tokenNum);

    COLOR_PRINT(CYAN, "type: '%s'\n", GetTokenTypeInStr(&tokenArr[tokenNum]));

    if (tokenArr[tokenNum].type == TokenType::TokenNumber_t)
    {
        Number number = tokenArr[tokenNum].data.number;
        COLOR_PRINT(CYAN, "data: '%lf'\n", number);
    }

    else
    {
        COLOR_PRINT(CYAN, "data: '%s'\n", GetTokenDataInStr(&tokenArr[tokenNum]));
    }

    COLOR_PRINT(GREEN, "\nToken Dump End.\n\n\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokenGraphicDump(const Token_t* tokenArr, size_t arrSize, const char* file, const int line, const char* func)
{
    assert(tokenArr);
    assert(file);
    assert(func);

    static size_t ImgQuant = 1;

    static const size_t MaxfileNameLen = 128;
    char outfile[MaxfileNameLen] = {};
    sprintf(outfile, "token%lu.png", ImgQuant);
    ImgQuant++;

    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    static const char* dotFileName = "token.dot";
    sprintf(command, "dot -Tpng %s > %s", dotFileName, outfile);
    
    TokenGraphicDumpHelper(tokenArr, arrSize, dotFileName, file, line, func);
    system(command);

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
        fprintf(dotFile, "%lf", number);
    }

    else if (type == TokenType::TokenName_t)
    {
        const char* name = token->data.name.name;
        size_t nameLen = token->data.name.nameLen;

        for (size_t i = 0; i < nameLen; i++)
        {
            fprintf(dotFile, "%c", name[i]);
        }
    }


    else
    {
        const char* tokenData  = GetTokenDataInStr(token);
        assert(tokenData);
        fprintf(dotFile, "%s", tokenData);
    }

    fprintf(dotFile, " | ");
    fprintf(dotFile, " token[%lu] | ", pointer + 1);
    fprintf(dotFile, " input::%lu::%lu } \", ", token->place.line, token->place.placeInLine);
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
        case TokenType::TokenName_t:        
        {
            NameType nameType = token->data.name.type;
            switch (nameType)
            {
                case NameType::Function: return "#f1481a";
                case NameType::Variable: return "#f31807";
                default: assert(0 && "undefined name type");
            }
        }
        case TokenType::TokenOperation_t:   return "#00ca2c";
        case TokenType::TokenFunction_t:    return "#0cf108";
        case TokenType::TokenBracket_t:     return "#e69c0c";
        case TokenType::TokenSeparator_t:   return "#fdc500";
        case TokenType::TokenEndSymbol_t:   return "#ffffff";
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
        case TokenType::TokenOperation_t: return "operation";
        case TokenType::TokenFunction_t:  return "function";
        case TokenType::TokenBracket_t:   return "bracket";
        case TokenType::TokenSeparator_t: return "separator";
        case TokenType::TokenEndSymbol_t: return "end";
        case TokenType::TokenName_t:
        {
            NameType nameType = token->data.name.type;
            switch (nameType)
            {
                case NameType::Variable: return "name/variable";
                case NameType::Function: return "name/function";
                default: assert(0 && "undef name type");
            }
        }
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
    
        case TokenType::TokenOperation_t:
        {
            Operation operation = token->data.operation;
            return GetOperationInStr(operation); //
        }
        case TokenType::TokenFunction_t:
        {
            Function function = token->data.function;
            return GetFuncInStr(function); //
        }
        case TokenType::TokenSeparator_t:
        {
            Separator separator = token->data.separator;

            switch (separator)
            {
                case Separator::comma:     return ",";
                case Separator::point:     return ".";
                case Separator::semicolon: return ";";
                case Separator::colon:     return ":";
                default: assert(0 && "undefined separetor type.");
            }
        }
        case TokenType::TokenBracket_t:
        {
            Bracket bracket = token->data.bracket;
            switch (bracket)
            {
                case Bracket::left: return "(";
                case Bracket::right: return ")";
                default: assert(0 && "undefined bracket type."); break;
            }
            break;
        }
    
        case TokenType::TokenEndSymbol_t:
        {
            EndSymbol end = token->data.end;
            switch (end)
            {
                case EndSymbol::end:  return "$";
                case EndSymbol::endd: return "\\\\0";
                default: assert(0 && "undefined end symbol.");
            }
            break;
        }
        default: assert(0 && "undefined type."); return "undefined";
    }

    return "undefined";
}

//=============================== Token Dump End =============================================================================================================================================