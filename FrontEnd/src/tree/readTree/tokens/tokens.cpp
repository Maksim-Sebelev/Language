#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include "lib/lib.hpp"
#include "tree/tree.hpp"
#include "tree/treeDump/treeDump.hpp"
#include "tree/readTree/syntaxErr/syntaxErr.hpp"
#include "tree/readTree/readTreeGlobalnclude.hpp"
#include "tree/readTree/tokens/token.hpp"

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

// static void HandleNumber       (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
// static void HandleOperation    (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
// static void HandleSeparator    (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
// static void HandleLetter       (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
// static void HandleBracket      (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);
// static void HandleEndSymbol    (const InputData* inputData, Token_t* tokenArr, Pointers* pointer);

// static void HandleName         (Token_t* tokenArr, Name       name    , Pointers* pointer, size_t wordSize);
// static void HandleFunction     (Token_t* tokenArr, DFunction  function, Pointers* pointer, size_t wordSize);
// static void HandleType         (Token_t* tokenArr, Type       type    , Pointers* pointer, size_t wordSize);


static bool IsPassSymbol       (char c);
static bool IsSpace            (char c);
static bool IsSlashN           (char c);
static bool IsSpaceOrSlashN    (char c);

static bool GetFlagPattern(const char* word, const char* defaultName, size_t defaultNameSize);


static void UpdatePointersAfterSpace  (Pointers* pointer);
static void UpdatePointersAfterSlashN (Pointers* pointer);


static bool IsEndSymbol                        (const char* input, size_t pointer);
static bool IsNumSymbol                        (const char* input, size_t pointer);
static bool IsOperationSymbol                  (const char* input, size_t pointer);
static bool IsSeparatorSymbol                  (const char* input, size_t pointer);
static bool IsLetterSymbol                     (const char* input, size_t pointer);
static bool IsLetterOrNumberOrUnderLineSymbol  (const char* input, size_t pointer);
static bool IsBracketSymbol                    (const char* input, size_t pointer);
static bool IsUnderLineSymbol                  (const char* input, size_t pointer);
static bool IsLetterOrUnderLineSymbol          (const char* input, size_t pointer);
static bool IsLetterOrNumberOrUnderLineSymbol  (const char* input, size_t pointer);

static Number    UpdateNumber     (Number number, const InputData* inputData, Pointers* pointer);

static Number    GetNumber        (const InputData* inputData, Pointers* pointer);

static Operation GetOperation     (const char* word, size_t* wordSize);
static Separator GetSeparator     (const char* word, size_t* wordSize);
static Type      GetType          (const char* word, size_t* wordSize);
static Cycle     GetCycle         (const char* word, size_t* wordSize);
static Bracket   GetBracket       (const char* word, size_t* wordSize);


static void CreateDefaultEndToken (      Token_t* tokenArr, Pointers* pointer);
static bool IsTokenTypeEnd        (const Token_t* token);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Token_t* ReadInputStr(const InputData* inputData, size_t inputLen, size_t* tokenArrSize)
{
    assert(inputData);
    assert(inputData->buffer);
    assert(inputData->inputStream);
    assert(tokenArrSize);

    const char* input = inputData->buffer;

    Token_t* tokenArr = (Token_t*) calloc(inputLen + 1, sizeof(*tokenArr));
    assert(tokenArr);

    Pointers pointer = {0, 0, 1, 1};

    while (pointer.ip < inputLen)
    {
        while (pointer.ip < inputLen && IsPassSymbol(input[pointer.ip]))
        {
            char tmp = input[pointer.ip];

            if      (IsSpace  (tmp)) UpdatePointersAfterSpace  (&pointer);
            else if (IsSlashN (tmp)) UpdatePointersAfterSlashN (&pointer);
            else                     assert(0 && "Something went wrong :(");
        }

        // if      (IsNumSymbol                  (input, pointer.ip))    HandleNumber    (inputData, tokenArr, &pointer);
        // else if (IsOperationSymbol            (input, pointer.ip))    HandleOperation (inputData, tokenArr, &pointer);
        // else if (IsSeparatorSymbol            (input, pointer.ip))    HandleSeparator (inputData, tokenArr, &pointer);
        // else if (IsLetterOrUnderLineSymbol    (input, pointer.ip))    HandleLetter    (inputData, tokenArr, &pointer);
        // else if (IsBracketSymbol              (input, pointer.ip))    HandleBracket   (inputData, tokenArr, &pointer);
        // else if (IsEndSymbol                  (input, pointer.ip))    HandleEndSymbol (inputData, tokenArr, &pointer);
        // else    SYNTAX_ERR(pointer.lp, pointer.sp, inputData, "undefined word in input.");

        const char* word     = inputData->buffer + pointer.ip;
        size_t      wordSize = 0;
    

        Operation operation = GetOperation(word, &wordSize);
        if (operation != Operation::undefined_operation)
        {
            // handle operation;
        }

        Separator separator = GetSeparator(word, &wordSize);
        if (separator != Separator::undefined_separator)
        {
            // handle separator
        }

        Type type = GetType(word, &wordSize);
        if (type != Type::undefined_type)
        {
            // handle type
        }

        Cycle cycle = GetCycle(word, &wordSize);
        if (cycle != Cycle::undefined_cycle)
        {
            // handle cycle
        }

        Bracket bracket = GetBracket(word, &wordSize);
        if (bracket != Bracket::undefined_bracket)
        {
            // handle bracket
        }


        Number num = GetNumber(inputData, &pointer);


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
        case TokenType::TokenType_t:      token->data.type     =  *(Type     *) value; break;
        case TokenType::TokenName_t:      token->data.name      = *(Name     *) value; break;
        case TokenType::TokenNumber_t:    token->data.number    = *(Number   *) value; break;
        case TokenType::TokenOperation_t: token->data.operation = *(Operation*) value; break;
        case TokenType::TokenSeparator_t: token->data.separator = *(Separator*) value; break;
        // case TokenType::TokenFunction_t:  token->data.function  = *(DFunction*) value; break;
        case TokenType::TokenBracket_t:   token->data.bracket   = *(Bracket  *) value; break;
        case TokenType::TokenEndSymbol_t: token->data.end       = *(EndSymbol*) value; break;

        default:                          assert(0 && "undefined token type symbol."); break;
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleNumber(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->buffer);
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
    // assert(inputData);
    // assert(inputData->buffer);
    // assert(inputData->inputStream);
    // assert(tokenArr);
    // assert(pointer);

    // const char* input = inputData->buffer;

    // size_t operationSize = 0;
    // Operation operation = GetOperation(input, pointer, &operationSize);
    // TokenCtor(&tokenArr[pointer->tp], TokenType::TokenOperation_t, &operation, pointer->lp, pointer->sp);
    // pointer->tp++;
    // pointer->ip++;
    // pointer->sp += operationSize;

    assert(inputData);
    assert(inputData->buffer);
    assert(tokenArr);
    assert(pointer);

    // const char* input = inputData->buffer;

    // size_t old_ip = pointer->ip;
    // do
    // {
    //     pointer->ip++;
    // }
    // while (IsOperationSymbol(input, pointer->ip));

    const char* word = inputData->buffer + pointer->ip;
    // const size_t wordSize = pointer->ip - old_ip;

    size_t wordSize = 0;

    Operation operation = GetOperation(word, &wordSize);

    if (operation == Operation::undefined_operation)
        SYNTAX_ERR(pointer->lp, pointer->sp, inputData, "undefined operator.");

    // COLOR_PRINT(RED, "token pointer = %lu\n", pointer->tp);
    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenOperation_t, &operation, pointer->lp, pointer->sp);

    pointer->tp++;

    pointer->sp += wordSize;
    pointer->ip += wordSize;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleSeparator(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(inputData);
    assert(inputData->buffer);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);

    const char* input = inputData->buffer;

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
    assert(inputData->buffer);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);

    const char* input = inputData->buffer;

    size_t old_ip = pointer->ip;
    do
    {
        pointer->ip++;
    }
    while (IsLetterOrNumberOrUnderLineSymbol(input, pointer->ip));

    const char*  word     = input + old_ip;
    const size_t wordSize = pointer->ip - old_ip;

    assert(word);

    // DFunction function = GetDFunction(word, wordSize);
    // if (function != DFunction::undefined_function)
    // {
        // HandleFunction(tokenArr, function, pointer, wordSize);
        // return;
    // }

    Type type = GetType(word, wordSize);
    if (type != Type::undefined_type)
    {
        HandleType(tokenArr, type, pointer, wordSize);
        return;
    }
    
    Name name = NameCtor(word, wordSize);
    HandleName(tokenArr, name, pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleBracket(const InputData* inputData, Token_t* tokenArr, Pointers* pointer)
{
    assert(tokenArr);
    assert(pointer);
    assert(IsBracketSymbol(inputData->buffer, pointer->ip));

    Bracket bracket = (Bracket) inputData->buffer[pointer->ip];
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
    assert(inputData->buffer);
    assert(inputData->inputStream);
    assert(tokenArr);
    assert(pointer);
    assert(IsEndSymbol(inputData->buffer, pointer->ip));

    const char* input = inputData->buffer;

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

// static void HandleFunction(Token_t* tokenArr, DFunction function, Pointers* pointer, size_t wordSize)
// {
    // assert(tokenArr);
    // assert(pointer);
// 
    // TokenCtor(&tokenArr[pointer->tp], TokenType::TokenFunction_t, &function, pointer->lp, pointer->sp);
// 
    // pointer->tp++;
    // pointer->sp += wordSize;
// 
    // return;
// }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleType(Token_t* tokenArr, Type type, Pointers* pointer, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenType_t, &type, pointer->lp, pointer->sp);

    pointer->tp++;
    pointer->sp += wordSize;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetNumber(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);


    Number number = {};

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
    assert(inputData->buffer);
    assert(inputData->inputStream);
    assert(pointer);

    const char* input = inputData->buffer;

    char new_digit_char = input[pointer->ip];

    assert('0' <= new_digit_char);
    assert('9' >= new_digit_char);


    Number new_digit = {};
    new_digit.int_val = new_digit_char - '0';


    assert(0 <= new_digit.int_val);
    assert(9 >= new_digit.int_val);


    if ((INT_MAX - new_digit.int_val) / 10 < number.int_val)
    {
        SYNTAX_ERR(pointer->lp, pointer->sp, inputData, "Integer overflow");
    }

    pointer->ip++;
    pointer->sp++;
    

    Number new_number = {};
    new_number.int_val = 10 * (number.int_val) + new_digit.int_val;

    return new_number;
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

static bool IsNumSymbol(char c)
{
    return ('0' <= c) && 
            (c <= '9');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsSpaceOrSlashN(char c)
{
    return  (c == ' ') ||
            (c == '\n');
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
            (c == '=') ||
            (c == '>') ||
            (c == '<') ||
            (c == '!');
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

static bool IsUnderLineSymbol(const char* input, size_t pointer)
{
    char c = input[pointer];
    return (c == '_');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterOrUnderLineSymbol(const char* input, size_t pointer)
{
    return IsLetterSymbol    (input, pointer) ||
           IsUnderLineSymbol (input, pointer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterOrNumberOrUnderLineSymbol(const char* input, size_t pointer)
{
    assert(input);

    return  IsLetterSymbol    (input, pointer) ||
            IsUnderLineSymbol (input, pointer) ||
            IsNumSymbol       (input, pointer);
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

static bool GetFlagPattern(const char* word, const char* defaultName, size_t defaultNameSize)
{
    return (strncmp(word, defaultName, defaultNameSize) == 0) &&
           (IsSpaceOrSlashN(word[defaultNameSize]));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Operation GetOperation(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t operation_i = 0; operation_i < DefaultOperationsQuant; operation_i++)
    {
        DefaultOperation operation   = DefaultOperations[operation_i];
    
        bool flag = GetFlagPattern(word, operation.name, operation.len);
        RETURN_IF_TRUE(flag, operation.value, *wordSize = operation.len);
    }

    return Operation::undefined_operation;
} 

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// #define STRNCMP(function) ((wordSize == strlen(name)) && (strncmp(word, function, wordSize) == 0))
// 
// static DFunction GetDFunction(const char* word, size_t wordSize)
// {
    // assert(word);
// 
    // for (size_t function_i = 0; function_i < DefaultFunctionsQuant; function_i++)
    // {
        // const char* name = DefaultFunctions[function_i].name;
        // DFunction   func = DefaultFunctions[function_i].value;
    // 
        // RETURN_IF_TRUE(STRNCMP(name), func);
    // }
// 
    // return DFunction::undefined_function;
// }

// #undef STRNCMP

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static Type GetType(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t type_i = 0; type_i < DefaultTypesQuant; type_i++)
    {
        DefaultType type = DefaultTypes[type_i];
    
        bool flag = GetFlagPattern(word, type.name, type.len);
        RETURN_IF_TRUE(flag, type.value, *wordSize = type.len);
    }

    return Type::undefined_type;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Separator GetSeparator(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t separator_i = 0; separator_i < DefaultSeparatorsQuant; separator_i++)
    {
        DefaultSeparator separator = DefaultSeparators[separator_i];
    
        bool flag = GetFlagPattern(word, separator.name, separator.len);
        RETURN_IF_TRUE(flag, separator.value, *wordSize = separator.len);
    }
    return Separator::undefined_separator;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Bracket GetBracket(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t bracket_i = 0; bracket_i < DefaultSeparatorsQuant; bracket_i++)
    {
        DefaultBracket bracket = DefaultBrackets[bracket_i];
    
        bool flag = GetFlagPattern(word, bracket.name, bracket.len);
        RETURN_IF_TRUE(flag, bracket.value, *wordSize = bracket.len);
    }
    return Bracket::undefined_bracket;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Cycle GetCycle(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t cycle_i = 0; cycle_i < DefaultCyclesQuant; cycle_i++)
    {
        DefaultCycle cycle = DefaultCycles[cycle_i];
    
        bool flag = GetFlagPattern(word, cycle.name, cycle.len);
        RETURN_IF_TRUE(flag, cycle.value, *wordSize = cycle.len);
    }
    return Cycle::undefined_cycle;
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
