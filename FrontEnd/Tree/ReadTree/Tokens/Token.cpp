#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include "../../../Common/GlobalInclude.hpp"
#include "../../Tree.hpp"
#include "../../TreeDump/TreeDump.hpp"
#include "../SyntaxErr/SyntaxErr.hpp"
#include "../ReadTreeGlobalnclude.hpp"
#include "Token.hpp"

//=============================== Tokens (Read Tree)  =======================================================================================================================================================================================

struct Pointers
{
    size_t ip; // input pointer
    size_t tp; // token pointer
    size_t lp; // line pointer (line in input file)
    size_t sp; // str pointer (pos in line)
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void TokenCtor        (Token_t* token, TokenType type, void* value, size_t fileLine, size_t linePos);

static void HandleNumber       (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
static void HandleOperation    (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
static void HandleSeparator    (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
static void HandleLetter       (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
static void HandleBracket      (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
static void HandleEndSymbol    (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
static void HandleName     (                      Token_t* tokenArr, Name  name, Pointers* pointer, size_t olp_sp);
static void HandleFunction     (                      Token_t* tokenArr, Function  function, Pointers* pointer, size_t old_sp);


static bool IsPassSymbol       (char c);
static bool IsSpace            (char c);
static bool IsSlashN           (char c);


static void UpdatePointersAfterSpace  (Pointers* pointer);
static void UpdatePointersAfterSlashN (Pointers* pointer);


static bool IsEndSymbol             (const char* input, size_t pointer);
static bool IsNumSymbol             (const char* input, size_t pointer);
static bool IsOperationSymbol       (const char* input, size_t pointer);
static bool IsSeparatorSymbol       (const char* input, size_t pointer);
static bool IsLetterSymbol          (const char* input, size_t pointer);
static bool IsLetterOrNumberSymbol  (const char* input, size_t pointer);
static bool IsBracketSymbol         (const char* input, size_t pointer);


static Number    UpdateNumber     (Number number, const InputData* inputData, Pointers* pointer);

static Number    GetNumber        (const InputData* inputData, Pointers* pointer);
static Operation GetOperation     (const char* operation,      Pointers* pointer, size_t* operationSize);
static Function  GetFunction      (const char* word,                              size_t  wordSize);
static Name      GetName          (const char* word,                              size_t  wordSize);


static void CreateDefaultEndToken (      Token_t* tokenArr, Pointers* pointer);
static bool IsTokenTypeEnd        (const Token_t* token);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Token_t* ReadInputStr(const InputData* inputData, size_t inputLen, size_t* tokenArrSize)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(tokenArrSize);

    const char* input = inputData->inputStr;

    Token_t* tokenArr = (Token_t*) calloc(inputLen + 1, sizeof(*tokenArr));
    assert(tokenArr);

    Pointers pointer = {0, 0, 1, 1};

    while (pointer.ip < inputLen)
    {
        while (pointer.ip < inputLen && IsPassSymbol(input[pointer.ip]))
        {
            char temp = input[pointer.ip];

            if      (IsSpace  (temp)) UpdatePointersAfterSpace  (&pointer);
            else if (IsSlashN (temp)) UpdatePointersAfterSlashN (&pointer);
            else                      assert(0 && "Something went wrong :(");
        }

        if      (IsNumSymbol       (input, pointer.ip))    HandleNumber    (inputData, tokenArr, &pointer);
        else if (IsOperationSymbol (input, pointer.ip))    HandleOperation (inputData, tokenArr, &pointer);
        else if (IsSeparatorSymbol (input, pointer.ip))    HandleSeparator (inputData, tokenArr, &pointer);
        else if (IsLetterSymbol    (input, pointer.ip))    HandleLetter    (inputData, tokenArr, &pointer);
        else if (IsBracketSymbol   (input, pointer.ip))    HandleBracket   (inputData, tokenArr, &pointer);
        else if (IsEndSymbol       (input, pointer.ip))    HandleEndSymbol (inputData, tokenArr, &pointer);
        else    SYNTAX_ERR(pointer.lp, pointer.sp, inputData, "undefined word in input.");

        if (pointer.ip == inputLen && (pointer.tp == 0 || !IsTokenTypeEnd(&tokenArr[pointer.tp - 1])))
        {
            CreateDefaultEndToken(tokenArr, &pointer);
            break;
        }
    }


    size_t tp = pointer.tp;

    if ((tp == 0) || (tp == 1))
    {
        FREE(tokenArr);
        return nullptr;
    }


    *tokenArrSize = pointer.tp;

    assert(*tokenArrSize > 0);
    assert(tokenArr); 
  
    tokenArr = (Token_t*) realloc(tokenArr, *tokenArrSize * sizeof(Token_t));

    assert(tokenArr);

    return tokenArr;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokenDtor(Token_t* tokenArr)
{
    assert(tokenArr);

    FREE(tokenArr);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void TokenCtor(Token_t* token, TokenType type, void* value, size_t fileLine, size_t linePos)
{
    assert(token);
    assert(value);

    token->type = type;

    token->place.line        = fileLine;
    token->place.placeInLine = linePos;

    switch (type)
    {
        case TokenType::TokenNumber_t:    token->data.number    = *(Number   *) value;   break;
        case TokenType::TokenName_t:      token->data.name      = *(Name     *) value;   break;
        case TokenType::TokenOperation_t: token->data.operation = *(Operation*) value;   break;
        case TokenType::TokenSeparator_t: token->data.separator = *(Separator*) value;   break;
        case TokenType::TokenFunction_t:  token->data.function  = *(Function *) value;   break;
        case TokenType::TokenBracket_t:   token->data.bracket   = *(Bracket  *) value;   break;
        case TokenType::TokenEndSymbol_t: token->data.end       = *(EndSymbol*) value;   break;
        default:                          assert(0 && "undefined token type symbol.");   break;
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleNumber(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);

    size_t old_sp = pointer->sp;
    Number number = GetNumber(inputData, pointer);
    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenNumber_t, &number, pointer->lp, old_sp);
    pointer->tp++;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleOperation(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);

    const char* input = inputData->inputStr;

    size_t operationSize = 0;
    Operation operation = GetOperation(input, pointer, &operationSize);
    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenOperation_t, &operation, pointer->lp, pointer->sp);
    pointer->tp++;
    pointer->ip++;
    pointer->sp += operationSize;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleSeparator(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);

    const char* input = inputData->inputStr;

    Separator separator = (Separator) input[pointer->ip];
    
    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenSeparator_t, &separator, pointer->lp, pointer->sp);
    pointer->tp++;
    pointer->ip++;
    pointer->sp++;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleLetter(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);

    const char* input = inputData->inputStr;

    size_t old_ip = pointer->ip;
    do
    {
        pointer->ip++;
    }
    while (IsLetterOrNumberSymbol(input, pointer->ip));

    const char* word = input + old_ip;
    const size_t wordSize = pointer->ip - old_ip;
    assert(word);

    Function function = GetFunction(word, wordSize);

    if (function != Function::undefined_function)
    {
        HandleFunction(tokenArr, function, pointer, wordSize);
        return;
    }

    Name name = GetName(word, wordSize);
    HandleName(tokenArr, name, pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleBracket(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(tokenArr);
    assert(pointer);
    assert(IsBracketSymbol(inputData->inputStr, pointer->ip));

    Bracket bracket = (Bracket) inputData->inputStr[pointer->ip];
    pointer->ip++;

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenBracket_t, &bracket, pointer->lp, pointer->sp);
    
    pointer->tp++;
    pointer->sp++;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleEndSymbol(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);
    assert(IsEndSymbol(inputData->inputStr, pointer->ip));

    const char* input = inputData->inputStr;

    EndSymbol end = (EndSymbol) input[pointer->ip];
    pointer->ip++;

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenEndSymbol_t, &end, pointer->lp, pointer->sp);
    pointer->tp++;
    pointer->sp++;

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleName(Token_t* tokenArr, Name name, Pointers* pointer, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenName_t, &name, pointer->lp, pointer->sp);

    pointer->tp++;
    pointer->sp += wordSize;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleFunction(Token_t* tokenArr, Function function, Pointers* pointer, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenFunction_t, &function, pointer->lp, pointer->sp);

    pointer->tp++;
    pointer->sp += wordSize;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetNumber(const InputData* inputData, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(pointer);
    assert(IsNumSymbol(inputData->inputStr, pointer->ip));

    const char* input = inputData->inputStr;

    Number number = 0;

    do
    {
        number = UpdateNumber(number, inputData, pointer);
    } 
    while (IsNumSymbol(input, pointer->ip));

    return number;    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number UpdateNumber(Number number, const InputData* inputData, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->inputStr);
    assert(inputData->inputStream);
    assert(pointer);

    const char* input = inputData->inputStr;

    char new_digit_char = input[pointer->ip];

    assert('0' <= new_digit_char);
    assert('9' >= new_digit_char);

    Number new_digit = new_digit_char - '0';

    assert(0 <= new_digit);
    assert(9 >= new_digit);


    if ((INT_MAX - new_digit) / 10 < number)
    {
        SYNTAX_ERR(pointer->lp, pointer->sp, inputData, "Integer overflow");
    }

    pointer->ip++;
    pointer->sp++;
    
    return 10 * number + new_digit;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static bool IsEndSymbol(const char* input, size_t pointer)
{
    assert(input);

    char temp = input[pointer];

    return  (temp == '$') ||
            (temp == '\0');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsNumSymbol(const char* input, size_t pointer)
{
    assert(input);

    return ('0' <= input[pointer]) && (input[pointer] <= '9');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsOperationSymbol(const char* input, size_t pointer)
{
    assert(input);

    char c = input[pointer];
    return  (c == '+') ||
            (c == '-') ||
            (c == '*') ||
            (c == '/') ||
            (c == '^') ||
            (c == '=');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsSeparatorSymbol(const char* input, size_t pointer)
{
    assert(input);

    char c = input[pointer];

    switch (c)
    {
        case Separator::comma:
        case Separator::point:
        case Separator::colon:
        case Separator::semicolon: break;
        default: return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterSymbol(const char* input, size_t pointer)
{
    assert(input);
    char c = input[pointer];

    return  ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterOrNumberSymbol(const char* input, size_t pointer)
{
    assert(input);
    char c = input[pointer];

    return  ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsBracketSymbol(const char* input, size_t pointer)
{
    assert(input);

    char c = input[pointer];
    return (c == '(') ||
           (c == ')');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsSpace(char c)
{
    return (c == ' ');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsSlashN(char c)
{
    return (c == '\n');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsPassSymbol(char c)
{
    return (IsSpace  (c) ||
            IsSlashN (c));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void UpdatePointersAfterSpace(Pointers* pointer)
{
    assert(pointer);

    pointer->ip++;
    pointer->sp++;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void UpdatePointersAfterSlashN(Pointers* pointer)
{
    assert(pointer);

    pointer->ip++;
    pointer->lp++;
    pointer->sp = 1;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define STRNCMP(oper) (strncmp(operation + pointer->ip, oper, strlen(oper)) == 0)

static Operation GetOperation(const char* operation, Pointers* pointer, size_t* operationSize)
{
    assert(operation);

    for (size_t operation_i = 0; operation_i < DefaultOperationsQuant; operation_i++)
    {
        const char* name = DefaultOperations[operation_i].name;
        Operation   oper = DefaultOperations[operation_i].value;

        RETURN_IF_TRUE(STRNCMP(name), oper, *operationSize = 1);
    }

    return Operation::undefined_operation;
} 

#undef STRNCMP

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define STRNCMP(function) (strncmp(word, function, wordSize) == 0 && wordSize == strlen(DefaultFunctions[function_i].name))

static Function GetFunction(const char* word, size_t wordSize)
{
    assert(word);

    for (size_t function_i = 0; function_i < DefaultFunctionsQuant; function_i++)
    {
        const char* name = DefaultFunctions[function_i].name;
        Function    func = DefaultFunctions[function_i].value;
    
        RETURN_IF_TRUE(STRNCMP(name), func);
    }

    return Function::undefined_function;
}

#undef STRNCMP

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Name GetName(const char* word, size_t wordSize)
{
    assert(word);

    Name name = {};
    name.id = 1;
    name.name = word;
    name.nameLen = wordSize;

    return name;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CreateDefaultEndToken(Token_t* tokenArr, Pointers* pointer)
{
    assert(tokenArr);
    assert(pointer);

    size_t    tp   = pointer->tp;

    TokenType type = TokenType::TokenEndSymbol_t;
    EndSymbol data = EndSymbol::endd;
    size_t    lp   = pointer->lp;
    size_t    sp   = pointer->sp;

    TokenCtor(&tokenArr[tp], type, &data, lp, sp);

    pointer->tp++;
    
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenTypeEnd(const Token_t* token)
{
    assert(token);

    TokenType type = token->type;

    return (type == TokenType::TokenEndSymbol_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
