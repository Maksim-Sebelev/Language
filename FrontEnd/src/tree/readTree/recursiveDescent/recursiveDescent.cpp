
#include <stdio.h>
#include <assert.h>
#include "tree/tree.hpp"
#include "tree/readTree/tokens/token.hpp"
#include "tree/readTree/syntaxErr/syntaxErr.hpp"
#include "tree/readTree/readTreeGlobalnclude.hpp"
#include "tree/treeDump/treeDump.hpp"
#include "tree/readTree/recursiveDescent/recursiveDescent.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetAssign            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetNumber            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetName              (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetType              (const Token_t* token, size_t* tp, const InputData* inputData);

static Node_t* GetBoolOperation     (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetAddSub            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetMulDiv            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetBracket           (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetPow               (const Token_t* token, size_t* tp, const InputData* inputData);

static Node_t* GetFunction          (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetMinus             (const Token_t* token, size_t* tp, const InputData* inputData);


static Number       GetTokenNumber     (const Token_t* token, const size_t* tp);
static Name         GetTokenName       (const Token_t* token, const size_t* tp);
static Type         GetTokenType       (const Token_t* token, const size_t* tp);
static Operation    GetTokenOperation  (const Token_t* token, const size_t* tp);
static DFunction    GetTokenFunction   (const Token_t* token, const size_t* tp);

static bool IsTokenEnd              (const Token_t* token, const size_t* tp);
static bool IsTokenNum              (const Token_t* token, const size_t* tp);
static bool IsTokenName             (const Token_t* token, const size_t* tp);
static bool IsTokenType             (const Token_t* token, const size_t* tp);
static bool IsTokenOperation        (const Token_t* token, const size_t* tp);
static bool IsTokenFunction         (const Token_t* token, const size_t* tp);


static bool IsTokenAssign           (const Token_t* token, const size_t* tp);
static bool IsTokenSemicolon        (const Token_t* token, const size_t* tp);
static bool IsBoolOperation         (const Token_t* token, const size_t* tp);
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
    Node_t* node = GetAssign(tokens, &tp, inputData);

    if (!IsTokenEnd(tokens, &tp))
        SYNTAX_ERR_FOR_TOKEN(tokens[tp], inputData, "expected '$'");

    tp++;

    TreeErr err = {};

    TREE_ASSERT(NODE_VERIF(node, err));

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetAssign(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    if (IsTokenEnd(token, tp))
    {
        return nullptr;
    }

    size_t old_tp = *tp;

    Node_t* type_node = GetType(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected type.");

    old_tp = *tp;
 
    Node_t* name_node = GetName(token, tp, inputData);
    type_node->left = name_node;

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected math variable.");

    if (!IsTokenAssign(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '='");

    (*tp)++;

    Node_t* node2 = GetBoolOperation(token, tp, inputData);

    if (!IsTokenSemicolon(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ';'");

    (*tp)++;


    Node_t* assign_node = {};
    _ASG(&assign_node, type_node, node2);

    TREE_ASSERT(SwapNode(&type_node, &assign_node));

    Node_t* next_assign_node = GetAssign(token, tp, inputData);

    Node_t* connect_node = {};
    _CONNECT(&connect_node, ';', type_node, next_assign_node);

    return connect_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetBoolOperation(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    size_t old_tp = *tp;

    Node_t* node = GetAddSub(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected math expression");


    while (IsBoolOperation(token, tp))
    {
        Operation operation = GetTokenOperation(token, tp);
        (*tp)++;
    
        Node_t* node2 = GetAddSub(token, tp, inputData);
    
        Node_t* new_node = {};
    
        assert(node);
        assert(node2);

        switch (operation)
        {
            case Operation::greater:             _GR  (&new_node, node, node2); break;
            case Operation::greater_or_equal:    _GROE(&new_node, node, node2); break;
            case Operation::less:                _LS  (&new_node, node, node2); break;
            case Operation::less_or_equal:       _LSOE(&new_node, node, node2); break;
            case Operation::equal:               _EQ  (&new_node, node, node2); break;
            case Operation::not_equal:           _NEQ (&new_node, node, node2); break;
            case Operation::undefined_operation:
            default:
            {
                assert(0 && "you forgot about some bool operation in get booo operation");
                break;
            }
        }

        TREE_ASSERT(SwapNode(&new_node, &node));
    }

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
        return GetMinus(token, tp, inputData);


    if (type != TokenType::TokenFunction_t)
        return GetBracket(token, tp, inputData);

    DFunction function = GetTokenFunction(token, tp);

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

    // consume token
    // pick token

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
        Node_t* node = GetBoolOperation(token, tp, inputData);
    
        if (!IsTokenRightBracket(token, tp)) 
            SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ')'");

        (*tp)++;  
        return node;
    }

    RETURN_IF_TRUE(IsTokenName(token, tp), GetName(token, tp, inputData));

    return GetNumber(token, tp, inputData);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetName(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);
    
    if (!IsTokenName(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expetcted variable name");

    Name name = GetTokenName(token, tp);
    (*tp)++;

    Node_t* node = {};
    _NAME(&node, name);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetNumber(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (!IsTokenNum(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected number.");

    Number val = GetTokenNumber(token, tp);
    (*tp)++;  

    Node_t* node = {};
    _NUM(&node, val);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetType(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (!IsTokenType(token, tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected type.");

    Type type = GetTokenType(token, tp);
    (*tp)++;

    Node_t* node = {};
    _TYPE(&node, type, nullptr);

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

    return (type == TokenType::TokenEndSymbol_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenNum(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::TokenNumber_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenName(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::TokenName_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenType(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::TokenType_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenOperation(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::TokenOperation_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenFunction(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    return (type == TokenType::TokenFunction_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenAssign(const Token_t* token, const size_t* tp)
{
    assert(token);

    Token_t temp = token[*tp];

    TokenType type = temp.type;

    return  (type == TokenType::TokenOperation_t) &&
            (temp.data.operation == Operation::assign);   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenSemicolon(const Token_t* token, const size_t* tp)
{
    assert(token);

    Token_t temp = token[*tp];

    TokenType type = temp.type;

    return  (type == TokenType::TokenSeparator_t) &&
            (temp.data.separator == Separator::semicolon);   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsBoolOperation(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type = TokenType::TokenOperation_t, false);

    Operation operation = token[*tp].data.operation;

    return  (operation == Operation::greater         ) ||
            (operation == Operation::greater_or_equal) ||
            (operation == Operation::less            ) ||
            (operation == Operation::less_or_equal   ) ||
            (operation == Operation::equal           ) ||
            (operation == Operation::not_equal       );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsAddSub(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::TokenOperation_t, false);

    Operation operation = token[*tp].data.operation;

    return (operation == Operation::plus) || (operation == Operation::minus);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsMulDiv(const Token_t* token, const size_t* tp)
{   
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::TokenOperation_t, false);

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

    RETURN_IF_FALSE(type == TokenType::TokenOperation_t, false);

    Operation operation = tokenCopy.data.operation;

    return (operation == Operation::power);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenLeftBracket (const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::TokenBracket_t, false);

    Bracket bracket = token[*tp].data.bracket;

    return (bracket == Bracket::left);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenMinus(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::TokenOperation_t, false);

    Operation operation = token[*tp].data.operation;

    return (operation == Operation::minus);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenRightBracket(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);

    TokenType type = token[*tp].type;

    RETURN_IF_FALSE(type == TokenType::TokenBracket_t, false);

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

    return (type == TokenType::TokenOperation_t);   
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

static Name GetTokenName(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenName(token, tp));

    Name name = token[*tp].data.name;

    return name;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Type GetTokenType(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenType(token, tp));

    Type type = token[*tp].data.type;

    return type;
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

static DFunction GetTokenFunction(const Token_t* token, const size_t* tp)
{
    assert(token);
    assert(tp);
    assert(IsTokenFunction(token, tp));

    DFunction function = token[*tp].data.function;
    return function;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
