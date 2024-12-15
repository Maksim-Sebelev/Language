
#include <stdio.h>
#include <assert.h>
#include "../../Tree.hpp"
#include "../Tokens/Token.hpp"
#include "../SyntaxErr/SyntaxErr.hpp"
#include "../ReadTreeGlobalnclude.hpp"
#include "RecursiveDescent.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetNumber            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetVariable          (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetAddSub            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetMulDiv            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetBracket           (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetPow               (const Token_t* token, size_t* tp, const InputData* inputData);

static Node_t* GetFunction          (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetMinus             (const Token_t* token, size_t* tp, const InputData* inputData);


static Number    GetTokenNumber     (const Token_t* token, const size_t* tp);
static Variable  GetTokenVariable   (const Token_t* token, const size_t* tp);
static Operation GetTokenOperation  (const Token_t* token, const size_t* tp);
static Function  GetTokenFunction   (const Token_t* token, const size_t* tp);

static bool IsTokenEnd              (const Token_t* token, const size_t* tp);
static bool IsTokenNum              (const Token_t* token, const size_t* tp);
static bool IsTokenVariable         (const Token_t* token, const size_t* tp);
static bool IsTokenOperation        (const Token_t* token, const size_t* tp);
static bool IsTokenFunction         (const Token_t* token, const size_t* tp);

static bool IsAddSub                (const Token_t* token, const size_t* tp);
static bool IsMulDiv                (const Token_t* token, const size_t* tp);
static bool IsPow                   (const Token_t* token, const size_t* tp);
static bool IsTokenLeftBracket      (const Token_t* token, const size_t* tp);
static bool IsTokenRightBracket     (const Token_t* token, const size_t* tp);
static bool IsOperationBeforeMinus  (const Token_t* token, const size_t* tp);
static bool IsTokenMinus            (const Token_t* token, const size_t* tp);
static bool IsOperationToken        (const Token_t* token,       size_t  tp);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Node_t* GetTree(const Token_t* tokens, const InputData* inputData)
{
    assert(tokens);

    size_t tp = 0;
    Node_t* node = GetAddSub(tokens, &tp, inputData);

    if (!IsTokenEnd(tokens, &tp))
        SYNTAX_ERR_FOR_TOKEN(tokens[tp], inputData, "expected '$'");

    tp++;

    TreeErr err = {};

    TREE_ASSERT(NODE_VERIF(node, err));

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetAddSub(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    size_t old_tp = *tp;

    Node_t* node = GetMulDiv(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected math expression");
    
    while(IsAddSub(token, tp))
    {
        Operation operation = GetTokenOperation(token, tp);
        (*tp)++;  

        Node_t* node2 = GetMulDiv(token, tp, inputData);
        Node_t* new_node = {};

        if (operation == Operation::plus)
        {
            _ADD(&new_node, node, node2);
        }
    
        else if (operation == Operation::minus)
        {
            _SUB(&new_node, node, node2);
        }
    
        else
        {
            assert(0 && "incorrect operation type");
        }

        TREE_ASSERT(SwapNode(&node, &new_node));
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetMulDiv(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    size_t old_tp = *tp;

    Node_t* node = GetPow(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected math expression");

    while (IsMulDiv(token, tp))
    {
        Operation operation = GetTokenOperation(token, tp);
        (*tp)++;  

        Node_t* node2 = GetPow(token, tp, inputData);
        Node_t* new_node = {};
    
        if (operation == Operation::mul)
        {
            _MUL(&new_node, node, node2);
        }

        else if (operation == Operation::dive)
        {
            _DIV(&new_node, node, node2);
        }

        else
        {
            assert(0 && "not a */ operation in get mul div");
        }

        TREE_ASSERT(SwapNode(&node, &new_node));
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetPow(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    Node_t* node = GetFunction(token, tp, inputData);

    while(IsPow(token, tp))
    {
        (*tp)++;  
        Node_t* node2 = GetFunction(token, tp, inputData);

        Node_t* new_node = {};
        _POW(&new_node, node, node2);
        TREE_ASSERT(SwapNode(&node, &new_node));
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetFunction(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    TokenType type = token[*tp].type;

    if (IsTokenMinus(token, tp))
    {
        return GetMinus(token, tp, inputData);
    }


    if (type != TokenType::Function_t)
    {
        return GetBracket(token, tp, inputData);
    }

    Function function = GetTokenFunction(token, tp);

    (*tp)++;  

    if (!IsTokenLeftBracket(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '('");

    (*tp)++;  

    Node_t* node = GetAddSub(token, tp, inputData);

    if (!IsTokenRightBracket(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ')'");

    (*tp)++;  

    Node_t* funcNode = {};

    _FUNC(&funcNode, function, node);

    TREE_ASSERT(SwapNode(&node, &funcNode));

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetMinus(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (IsOperationBeforeMinus(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "Operation before '-'");

    (*tp)++;  

    size_t old_tp = *tp;

    Node_t* node = GetMulDiv(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "Nothig after '-'");

    Node_t* new_node = {};
    _SUB(&new_node, node, nullptr);

    TREE_ASSERT(SwapNode(&node, &new_node));

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetBracket(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    if (IsTokenLeftBracket(token, tp))
    {
        (*tp)++;  
        Node_t* node = GetAddSub(token, tp, inputData);
    
        if (!IsTokenRightBracket(token, tp)) 
            SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ')'");

        (*tp)++;  
        return node;
    }

    RETURN_IF_TRUE(IsTokenVariable(token, tp), GetVariable(token, tp, inputData));

    return GetNumber(token, tp, inputData);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetVariable(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);
    
    if (!IsTokenVariable(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expetcted variable name");

    Variable variable = GetTokenVariable(token, tp);
    (*tp)++;

    Node_t* node = {};
    _VAR(&node, variable);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetNumber(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (!IsTokenNum(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected math expression");
    
    Number val = GetTokenNumber(token, tp);
    (*tp)++;  

    Node_t* node = {};
    _NUM(&node, val);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenEnd(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::EndSymbol_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenNum(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::Number_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenVariable(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::Variable_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenOperation(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::Operation_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenFunction(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::Function_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsAddSub(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::Operation_t, false);

    Operation operation = token[*tp].data.operation;

    return (operation == Operation::plus) || (operation == Operation::minus);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsMulDiv(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::Operation_t, false);

    Operation operation = token[*tp].data.operation;

    return (operation == Operation::mul) || (operation == Operation::dive);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsPow(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);
    assert(token + *tp);

    Token_t tokenCopy = token[*tp];

    TokenType type = tokenCopy.type;

    RETURN_IF_FALSE(type == TokenType::Operation_t, false);

    Operation operation = tokenCopy.data.operation;

    return (operation == Operation::power);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenLeftBracket (const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::Bracket_t, false);

    Bracket bracket = token[*tp].data.bracket;

    return (bracket == Bracket::left);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenMinus(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::Operation_t, false);

    Operation operation = token[*tp].data.operation;

    return (operation == Operation::minus);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenRightBracket(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::Bracket_t, false);

    Bracket bracket = token[*tp].data.bracket;

    return (bracket == Bracket::right);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsOperationBeforeMinus(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsOperationToken(token, *tp));
    assert(token[*tp].data.operation == Operation::minus);

    return (*tp >= 1 && IsOperationToken(token, *tp - 1));   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsOperationToken(const Token_t* token, size_t tp)
{
    assert(token);

    TokenType type = token[tp].type;

    return (type == TokenType::Operation_t);   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetTokenNumber(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenNum(token, tp));

    Number number = token[*tp].data.number;
    return number;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Variable GetTokenVariable(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenVariable(token, tp));

    Variable variable = token[*tp].data.variable;

    return variable;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Operation GetTokenOperation(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenOperation(token, tp));

    Operation operation = token[*tp].data.operation;
    return operation;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Function GetTokenFunction(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenFunction(token, tp));

    Function function = token[*tp].data.function;
    return function;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
