#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "lib/lib.hpp"
#include "read-tree/tokens/tokens.hpp"
#include "tree/node-and-token-types.hpp"
#include "read-tree/file-read/file-read.hpp"
#include "read-tree/syntax-err/syntax-err.hpp"


#ifdef _DEBUG
#include "log/log.hpp"
#endif

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


static void HandleName               (Token_t* tokenArr, Pointers* pointer, Name         name                      );
static void HandleType               (Token_t* tokenArr, Pointers* pointer, Type         type     , size_t wordSize);
static void HandleCycle              (Token_t* tokenArr, Pointers* pointer, Cycle        cycle    , size_t wordSize);
static void HandleNumber             (Token_t* tokenArr, Pointers* pointer, Number       number   , size_t wordSize);
static void HandleBracket            (Token_t* tokenArr, Pointers* pointer, Bracket      bracket  , size_t wordSize);
static void HandleSeparator          (Token_t* tokenArr, Pointers* pointer, Separator    separator, size_t wordSize);
static void HandleOperation          (Token_t* tokenArr, Pointers* pointer, Operation    operation, size_t wordSize);
static void HandleCondition          (Token_t* tokenArr, Pointers* pointer, Condition    condition, size_t wordSize);
static void HandleDefaultFunction    (Token_t* tokenArr, Pointers* pointer, DFunction    function , size_t wordSize);
static void HandleFunctionAttribute  (Token_t* tokenArr, Pointers* pointer, FunctionAttribute attribute, size_t wordSize);


static bool HandleComment       (const char* word, Pointers* pointer);

static bool IsPassSymbol       (char c);
static bool IsSpace            (char c);
static bool IsSlashN           (char c);
static bool IsSlash0           (char c);
static bool IsSlashNOrSlashN   (char c);

static bool GetFlagPattern(const char* word, const char* defaultName, size_t defaultNameSize);


static void UpdatePointersAfterSpace  (Pointers* pointer);
static void UpdatePointersAfterSlashN (Pointers* pointer);


static bool IsNumSymbol                        (char c);
static bool IsLetterSymbol                     (char c);
static bool IsLetterOrNumberOrUnderLineSymbol  (char c);
static bool IsUnderLineSymbol                  (char c);
static bool IsLetterOrUnderLineSymbol          (char c);

static Name              GetName              (const char* word                  );
static DFunction         GetDefaultFucntion   (const char* word, size_t* wordSize);
static Number            GetInt               (const char* word, size_t* wordSize);
static Operation         GetOperation         (const char* word, size_t* wordSize);
static Separator         GetSeparator         (const char* word, size_t* wordSize);
static Type              GetType              (const char* word, size_t* wordSize);
static Cycle             GetCycle             (const char* word, size_t* wordSize);
static Condition         GetCondition         (const char* word, size_t* wordSize);
static Bracket           GetBracket           (const char* word, size_t* wordSize);
static Number            GetNumber            (const char* word, size_t* wordSize);
static Number            GetDouble            (const char* word, size_t* wordSize);
static Number            GetChar              (const char* word, size_t* wordSize);
static FunctionAttribute GetFunctionAttribute (const char* word, size_t* wordSize);

static void CreateDefaultEndToken (Token_t* tokenArr, Pointers* pointer);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TokensArr ReadInputBuffer (const InputData* inputData)
{
    assert(inputData);
    assert(inputData->buffer);
    assert(inputData->inputStream);

    const char* input       = inputData->buffer;
    size_t      buffer_size = inputData->size;

    Token_t* tokens = (Token_t*) calloc(buffer_size + 1, sizeof(*tokens));

    if (!tokens)
        EXIT(EXIT_FAILURE, "failed calloc memory for tokens array.");

    Pointers pointer = {0, 0, 1, 1};

    while (pointer.ip < buffer_size)
    {
        while (pointer.ip < buffer_size && IsPassSymbol(input[pointer.ip]))
        {
            char tmp = input[pointer.ip];

            if      (IsSpace  (tmp)) UpdatePointersAfterSpace  (&pointer);
            else if (IsSlashN (tmp)) UpdatePointersAfterSlashN (&pointer);
            else                     assert(0 && "Something went wrong :(");
        }

        const char* word     = inputData->buffer + pointer.ip;
        size_t      wordSize = 0;

        if (IsSlash0(word[0])) break;

        if (HandleComment(word, &pointer))
            continue;


        Operation operation = GetOperation(word, &wordSize);
        if (operation != Operation::undefined_operation)
        {
            HandleOperation(tokens, &pointer, operation, wordSize);
            continue;
        }

        DFunction function = GetDefaultFucntion(word, &wordSize);
        if (function != DFunction::undefined_function)
        {
            HandleDefaultFunction(tokens, &pointer, function, wordSize);
            continue;
        }

        Number number = GetNumber(word, &wordSize);
        if (number.type != Type::undefined_type)
        {
            HandleNumber(tokens, &pointer, number, wordSize);
            continue;
        }


        Condition condition = GetCondition(word, &wordSize);
        if (condition != Condition::undefined_condition)
        {
            HandleCondition(tokens, &pointer, condition, wordSize);
            continue;
        }
    
        Type type = GetType(word, &wordSize);
        if (type != Type::undefined_type)
        {
            HandleType(tokens, &pointer, type, wordSize);
            continue;
        }


        Cycle cycle = GetCycle(word, &wordSize);
        if (cycle != Cycle::undefined_cycle)
        {
            HandleCycle(tokens, &pointer, cycle, wordSize);
            continue;
        }


        Bracket bracket = GetBracket(word, &wordSize);
        if (bracket != Bracket::undefined_bracket)
        {
            HandleBracket(tokens, &pointer, bracket, wordSize);
            continue;
        }

        FunctionAttribute attribute = GetFunctionAttribute(word, &wordSize);
        if (attribute != FunctionAttribute::undefined_attribute)
        {
            HandleFunctionAttribute(tokens, &pointer, attribute, wordSize);
            continue;
        }

        Separator separator = GetSeparator(word, &wordSize);
        if (separator != Separator::undefined_separator)
        {
            HandleSeparator(tokens, &pointer, separator, wordSize);
            continue;
        }


        Name name = GetName(word);
        if (name.name.len > 0)
        {
            HandleName(tokens, &pointer, name);
            continue;
        }


        SYNTAX_ERR(pointer.lp, pointer.sp, inputData, "undefined word.");
    }

    CreateDefaultEndToken(tokens, &pointer);

    size_t tokens_arr_size = pointer.tp;

    tokens = (Token_t*) realloc(tokens, tokens_arr_size * sizeof(Token_t));
    if (!tokens)
        EXIT(EXIT_FAILURE, "failed realloc memory for tokens arr.");


    TokensArr tokensArr = {};
    tokensArr.arr  = tokens;
    tokensArr.size = tokens_arr_size;

    return tokensArr;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TokenDtor(TokensArr* tokenArr)
{
    assert(tokenArr);

    FREE(tokenArr->arr);
    tokenArr->size = 0;

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
        case TokenType::TokenType_t:        token->data.type      = *(Type             *) value; break;
        case TokenType::TokenName_t:        token->data.name      = *(Name             *) value; break;
        case TokenType::TokenNumber_t:      token->data.number    = *(Number           *) value; break;
        case TokenType::TokenOperation_t:   token->data.operation = *(Operation        *) value; break;
        case TokenType::TokenSeparator_t:   token->data.separator = *(Separator        *) value; break;
        case TokenType::TokenBracket_t:     token->data.bracket   = *(Bracket          *) value; break;
        case TokenType::TokenEndSymbol_t:   token->data.end       = *(EndSymbol        *) value; break;
        case TokenType::TokenCondition_t:   token->data.condition = *(Condition        *) value; break;
        case TokenType::TokenCycle_t:       token->data.cycle     = *(Cycle            *) value; break;
        case TokenType::TokenFuncAttr_t:    token->data.attribute = *(FunctionAttribute*) value; break;
        case TokenType::TokenDefaultFunc_t: token->data.function  = *(DFunction        *) value; break;
        default:                          assert(0 && "undefined token type symbol."); break;
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void UpdatePointer(Pointers* pointer, size_t wordSize)
{
    assert(pointer);
    pointer->ip += wordSize;
    pointer->sp += wordSize;
    pointer->tp++;

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleDefaultFunction(Token_t* tokenArr, Pointers* pointer, DFunction function, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenDefaultFunc_t, &function, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleOperation(Token_t* tokenArr, Pointers* pointer, Operation operation, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenOperation_t, &operation, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleSeparator(Token_t* tokenArr, Pointers* pointer, Separator separator, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenSeparator_t, &separator, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleCondition(Token_t* tokenArr, Pointers* pointer, Condition condition, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenCondition_t, &condition, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleType(Token_t* tokenArr, Pointers* pointer, Type type, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenType_t, &type, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleCycle(Token_t* tokenArr, Pointers* pointer, Cycle cycle, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenCycle_t, &cycle, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleBracket(Token_t* tokenArr, Pointers* pointer, Bracket bracket, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenBracket_t, &bracket, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleName(Token_t* tokenArr, Pointers* pointer, Name name)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenName_t, &name, pointer->lp, pointer->sp);

    UpdatePointer(pointer, name.name.len);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleNumber(Token_t* tokenArr, Pointers* pointer, Number number, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenNumber_t, &number, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HandleFunctionAttribute(Token_t* tokenArr, Pointers* pointer, FunctionAttribute attribute, size_t wordSize)
{
    assert(tokenArr);
    assert(pointer);

    TokenCtor(&tokenArr[pointer->tp], TokenType::TokenFuncAttr_t, &attribute, pointer->lp, pointer->sp);

    UpdatePointer(pointer, wordSize);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static DFunction GetDefaultFucntion(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t dfunction_i = 0; dfunction_i < DefaultFunctionsQuant; dfunction_i++)
    {
        DefaultFunction dfunction = DefaultFunctions[dfunction_i];

        bool flag = GetFlagPattern(word, dfunction.nameInfo.name, dfunction.nameInfo.len);
        RETURN_IF_TRUE(flag, dfunction.value, *wordSize = dfunction.nameInfo.len);
    }

    return DFunction::undefined_function;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetNumber(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    Number number = {};

    
    number = GetInt(word, wordSize);
    RETURN_IF_TRUE(number.type == Type::int_type, number);
    
    number = GetDouble(word, wordSize);
    RETURN_IF_TRUE(number.type == Type::double_type, number);

    number = GetChar(word, wordSize);
    RETURN_IF_TRUE(number.type == Type::char_type, number);

    number = {.type = Type::undefined_type};
    return number;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetInt(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);


    Number number = {};

    char* numEnd = nullptr;
    number.value.int_val = (int) strtol(word, &numEnd, 10);
    size_t size = (size_t) (numEnd - word);

    if (((size == 1 && (word[0] != '+' && word[0] != '-')) || size > 1) && word[size] != '.')
        number.type = Type::int_type;

    *wordSize = size;

    return number; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetDouble(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    Number number = {};

    char* numEnd = nullptr;
    number.value.double_val = strtod(word, &numEnd);
    size_t size = (size_t) (numEnd - word);

    if ((size == 1 && (word[0] != '+' && word[0] != '-')) || size > 1)
        number.type = Type::double_type;

    *wordSize = size;

    return number;    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetChar(const char* word, size_t* wordSize)
{

    Number number = {}; 

    number.value.char_val = word[*wordSize];  /// TODO
    return number;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Name GetName(const char* word)
{
    assert(word);
    
    Name name = {};

    if (!IsLetterOrUnderLineSymbol(word[0]))
    {
        name.type = NameType::undefined_name_type;
        return name;
    }
    
    size_t i = 1;
    while (IsLetterOrNumberOrUnderLineSymbol(word[i])) i++;

    name.name.len = i;
    name.name.name = word;
    return name;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Operation GetOperation(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);
    
    for (size_t operation_i = 0; operation_i < DefaultOperationsQuant; operation_i++)
    {
        DefaultOperation operation = DefaultOperations[operation_i];
    
        bool flag = GetFlagPattern(word, operation.nameInfo.name, operation.nameInfo.len);
        RETURN_IF_TRUE(flag, operation.value, *wordSize = operation.nameInfo.len);
    }
    
    return Operation::undefined_operation;
} 

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Type GetType(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t type_i = 0; type_i < DefaultTypesQuant; type_i++)
    {
        DefaultType type = DefaultTypes[type_i];
        
        bool flag = GetFlagPattern(word, type.nameInfo.name, type.nameInfo.len);
        RETURN_IF_TRUE(flag, type.value, *wordSize = type.nameInfo.len);
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
        
        bool flag = GetFlagPattern(word, separator.nameInfo.name, separator.nameInfo.len);
        RETURN_IF_TRUE(flag, separator.value, *wordSize = separator.nameInfo.len);
    }
    return Separator::undefined_separator;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Bracket GetBracket(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);
    
    for (size_t bracket_i = 0; bracket_i < DefaultBracketsQuant; bracket_i++)
    {
        DefaultBracket bracket = DefaultBrackets[bracket_i];
        
        bool flag = GetFlagPattern(word, bracket.nameInfo.name, bracket.nameInfo.len);
        RETURN_IF_TRUE(flag, bracket.value, *wordSize = bracket.nameInfo.len);
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
    
        bool flag = GetFlagPattern(word, cycle.nameInfo.name, cycle.nameInfo.len);
        RETURN_IF_TRUE(flag, cycle.value, *wordSize = cycle.nameInfo.len);
    }
    return Cycle::undefined_cycle;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Condition GetCondition(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t condition_i = 0; condition_i < DefaultConditionsQuant; condition_i++)
    {
        DefaultCondition condition = DefaultConditions[condition_i];
    
        bool flag = GetFlagPattern(word, condition.nameInfo.name, condition.nameInfo.len);
        RETURN_IF_TRUE(flag, condition.value, *wordSize = condition.nameInfo.len);
    }
    return Condition::undefined_condition;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static FunctionAttribute GetFunctionAttribute(const char* word, size_t* wordSize)
{
    assert(word);
    assert(wordSize);

    for (size_t condition_i = 0; condition_i < DefaultFunctionAttributesQuant; condition_i++)
    {
        DefaultFunctionAttribute atrribute = DefaultFunctionsAttributes[condition_i];
    
        bool flag = GetFlagPattern(word, atrribute.nameInfo.name, atrribute.nameInfo.len);
        RETURN_IF_TRUE(flag, atrribute.value, *wordSize = atrribute.nameInfo.len);
    }
    return FunctionAttribute::undefined_attribute;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool HandleComment(const char* word, Pointers* pointer)
{
    assert(word);
    assert(pointer);
    
    bool flag = false;
    size_t affterCommentStrLen = 0;
    for (size_t i = 0; i < OneLineCommentsQuant; i++)
    {
        OneLineComment comment = OneLineComments[i];
        
        flag = GetFlagPattern(word, comment.name, comment.len);
        if (flag)
        {
            affterCommentStrLen = comment.len;
            break;
        }
    }

    if (!flag) return false;
    while (!IsSlashNOrSlashN(word[affterCommentStrLen])) 
    affterCommentStrLen++;
    
    pointer->ip += affterCommentStrLen;
    pointer->sp += affterCommentStrLen;
    
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsNumSymbol(char c)
{
    return ('0' <= c) && 
            (c <= '9');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterSymbol(char c)
{
    return  ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsUnderLineSymbol(char c)
{
    return (c == '_');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterOrUnderLineSymbol(char c)
{
    return  IsLetterSymbol   (c) ||
            IsUnderLineSymbol(c);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsLetterOrNumberOrUnderLineSymbol(char c)
{
    return  IsLetterSymbol    (c) ||
            IsUnderLineSymbol (c) ||
            IsNumSymbol       (c);
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

static bool IsSlash0(char c)
{
    return (c == '\0');
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsSlashNOrSlashN(char c)
{
    return  IsSlashN(c) ||
            IsSlash0(c);
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
    return (strncmp(word, defaultName, defaultNameSize) == 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
