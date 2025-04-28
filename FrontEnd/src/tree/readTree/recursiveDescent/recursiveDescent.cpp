
#include <stdio.h>
#include <assert.h>
#include "tree/tree.hpp"
#include "tree/readTree/tokens/token.hpp"
#include "tree/readTree/syntaxErr/syntaxErr.hpp"
#include "tree/readTree/readTreeGlobalnclude.hpp"
#include "tree/treeDump/treeDump.hpp"
#include "tree/readTree/recursiveDescent/recursiveDescent.hpp"
#include "log/log.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetIfCondition       (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetElseIfCondition   (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetElseCondition     (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetAssign            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetNumber            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetName              (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetType              (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetBoolOperation     (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetAddSub            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetMulDiv            (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetBracket           (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetPow               (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetNot               (const Token_t* token, size_t* tp, const InputData* inputData);
static Node_t* GetMinus             (const Token_t* token, size_t* tp, const InputData* inputData);


static Number       GetTokenNumber           (const Token_t* token);
static Name         GetTokenName             (const Token_t* token);
static Type         GetTokenType             (const Token_t* token);
static Operation    GetTokenOperation        (const Token_t* token);

static bool IsTokenEnd                       (const Token_t* token);
static bool IsTokenNum                       (const Token_t* token);
static bool IsTokenName                      (const Token_t* token);
static bool IsTokenType                      (const Token_t* token);
static bool IsTokenOperation                 (const Token_t* token);
static bool IsTokenFunction                  (const Token_t* token);


static bool IsTokenAssign                    (const Token_t* token);
static bool IsTokenSemicolon                 (const Token_t* token);
static bool IsBoolOperation                  (const Token_t* token);
static bool IsAddSub                         (const Token_t* token);
static bool IsMulDiv                         (const Token_t* token);
static bool IsPow                            (const Token_t* token);
static bool IsTokenLeftRoundBracket          (const Token_t* token);
static bool IsTokenRightRoundBracket         (const Token_t* token);
static bool IsTokenMinus                     (const Token_t* token);
static bool IsOperationToken                 (const Token_t* token);
static bool IsTokenConditionIf               (const Token_t* token);
static bool IsTokenConditionElseIf           (const Token_t* token);
static bool IsTokenConditionElse             (const Token_t* token);
static bool IsTokenOperationNot              (const Token_t* token);
static bool IsTokenLeftCurlyBracket          (const Token_t* token);
static bool IsTokenRightCurlyBracket         (const Token_t* token);
static bool IsNotAssignOperationBeforeMinus  (const Token_t* token, size_t tp);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Node_t* GetTree(const Token_t* tokens, const InputData* inputData)
{
    assert(tokens);

    size_t tp = 0;
    Node_t* node = GetIfCondition(tokens, &tp, inputData);

    if (!IsTokenEnd(&tokens[tp]))
        SYNTAX_ERR_FOR_TOKEN(tokens[tp], inputData, "expected '$'");

    tp++;


    TreeErr err = {};

    TREE_ASSERT(NODE_VERIF(node, err));

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetIfCondition(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (IsTokenEnd(&token[*tp]) || IsTokenRightCurlyBracket(&token[*tp]))
        return nullptr;

    if (!IsTokenConditionIf(&token[*tp]))
    {
        Node_t* assign_node = GetAssign(token, tp, inputData);
        Node_t* next_if_condition_node = GetIfCondition(token, tp, inputData);
        Node_t* main_connect_node = {};
        _CONNECT(&main_connect_node, assign_node, next_if_condition_node);
        return main_connect_node;
    }

    (*tp)++;
 
    if (!IsTokenLeftRoundBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected' '(");

    (*tp)++;

    Node_t* if_node = {};

    size_t old_tp = *tp;

    Node_t* bool_node = GetBoolOperation(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected bool");

    if (!IsTokenRightRoundBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ')'");
    
    (*tp)++;

    if (!IsTokenLeftCurlyBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '{'");

    (*tp)++;

    old_tp = *tp;

    Node_t* body_node = GetIfCondition(token, tp, inputData);

    if (!IsTokenRightCurlyBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '}'");
    
    (*tp)++;

    _IF(&if_node, bool_node, body_node);

    Node_t* else_if_node = GetElseIfCondition(token, tp, inputData);
    Node_t* else_node    = GetElseCondition   (token, tp, inputData);
    

    Node_t* connect_node = {};

    if (!else_node && else_if_node)
        _CONNECT(&connect_node, if_node, else_if_node);

    else if (else_node && !else_if_node)
        _CONNECT(&connect_node, if_node, else_node);

    else if (else_node && else_if_node)
    {
        Node_t* connect_node_2 = {};
        _CONNECT(&connect_node_2, else_if_node, else_node     );
        _CONNECT(&connect_node  , if_node     , connect_node_2);
    }

    else
    {
        Node_t* next_if_condition = GetIfCondition(token, tp, inputData);
        _CONNECT(&connect_node, if_node, next_if_condition);
        return connect_node;
    }
    
    Node_t* main_connect_node = {};
    Node_t* next_if_condition = GetIfCondition(token, tp, inputData);
    _CONNECT(&main_connect_node, connect_node, next_if_condition);
    
    return main_connect_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetElseIfCondition(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (!IsTokenConditionElseIf(&token[*tp]))
        return nullptr;
    
    (*tp)++;

    Node_t* else_if = {};

    if (!IsTokenLeftRoundBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '('");

    (*tp)++;

    Node_t* bool_node = GetBoolOperation(token, tp, inputData);

    if (!IsTokenRightRoundBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ')'");

    (*tp)++;

    if (!IsTokenLeftCurlyBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '{'");

    (*tp)++;

    Node_t* body_node = GetIfCondition(token, tp, inputData);

    if (!IsTokenRightCurlyBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '}'");
    
    (*tp)++;

    Node_t* else_if_node = {};
    _ELIF(&else_if_node, bool_node, body_node);

    Node_t* next_else_if_node = GetElseIfCondition(token, tp, inputData);
    
    Node_t* connect_node = {};
    _CONNECT(&connect_node, else_if_node, next_else_if_node);

    return connect_node;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetElseCondition(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (!IsTokenConditionElse(&token[*tp]))
        return nullptr;

    (*tp)++;

    if (!IsTokenLeftCurlyBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '{'");

    (*tp)++;

    Node_t* body_node = GetIfCondition(token, tp, inputData);

    if (!IsTokenRightCurlyBracket(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '}'");

    (*tp)++;

    Node_t* else_node = {};
    _ELSE(&else_node, body_node);

    return else_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static Node_t* GetAssign(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    Node_t* type_node = {};
    
    bool wasType = false;

    if (wasType = IsTokenType(&token[*tp]))
        type_node = GetType(token, tp, inputData);

    size_t old_tp = *tp;
    
    Node_t* name_node = GetName(token, tp, inputData);

    if (wasType)
        type_node->left = name_node;

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected math variable.");

    if (!IsTokenAssign(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected '='");

    (*tp)++;

    Node_t* node2 = GetBoolOperation(token, tp, inputData);

    if (!IsTokenSemicolon(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ';'");

    (*tp)++;
    
    Node_t* assign_node = {};
    Node_t* connect_node = {};

    if (wasType)
        _ASG(&assign_node, type_node, node2);
    else
        _ASG(&assign_node, name_node, node2);

    return assign_node;
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


    while (IsBoolOperation(&token[*tp]))
    {
        Operation operation = GetTokenOperation(&token[*tp]);
        (*tp)++;
    
        Node_t* node2 = GetAddSub(token, tp, inputData);
    
        Node_t* new_node = {};
    
        switch (operation)
        {
            case Operation::greater:             _GR  (&new_node, node, node2); break;
            case Operation::greater_or_equal:    _GROE(&new_node, node, node2); break;
            case Operation::less:                _LS  (&new_node, node, node2); break;
            case Operation::less_or_equal:       _LSOE(&new_node, node, node2); break;
            case Operation::equal:               _EQ  (&new_node, node, node2); break;
            case Operation::not_equal:           _NEQ (&new_node, node, node2); break;
            case Operation::bool_and:            _AND (&new_node, node, node2); break;
            case Operation::bool_or:             _OR  (&new_node, node, node2); break;
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
    
    while(IsAddSub(&token[*tp]))
    {
        Operation operation = GetTokenOperation(&token[*tp]);
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

    while (IsMulDiv(&token[*tp]))
    {
        Operation operation = GetTokenOperation(&token[*tp]);
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

    Node_t* node = GetNot(token, tp, inputData);

    while(IsPow(&token[*tp]))
    {
        (*tp)++;  
        Node_t* node2 = GetNot(token, tp, inputData);

        Node_t* new_node = {};
        _POW(&new_node, node, node2);
        TREE_ASSERT(SwapNode(&node, &new_node));
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetNot(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    if (!IsTokenOperationNot(&token[*tp]))
        return GetMinus(token, tp, inputData);
    
    (*tp)++;

    size_t old_tp = *tp;

    Node_t* node = GetMinus(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "nothing after '!'");

    Node_t* not_node = {};
    _NOT(&not_node, node);

    return not_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetMinus(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(token);
    assert(tp);

    // consume token
    // pick token

    if (!IsTokenMinus(&token[*tp]))
        return GetBracket(token, tp, inputData);

    if (IsNotAssignOperationBeforeMinus(token, *tp))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "Operation before '-'");

    (*tp)++;  

    size_t old_tp = *tp;

    Node_t* node = GetMulDiv(token, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "Nothig after '-'");

    Node_t* new_node = {};
    _SUB(&new_node, node, nullptr);

    // TREE_ASSERT(SwapNode(&node, &new_node));

    return new_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetBracket(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);

    if (IsTokenLeftRoundBracket(&token[*tp]))
    {
        (*tp)++;  
        Node_t* node = GetBoolOperation(token, tp, inputData);
    
        if (!IsTokenRightRoundBracket(&token[*tp])) 
            SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected ')'");

        (*tp)++;  
        return node;
    }

    RETURN_IF_TRUE(IsTokenName(&token[*tp]), GetName(token, tp, inputData));

    return GetNumber(token, tp, inputData);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetName(const Token_t* token, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(token);
    
    if (!IsTokenName(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expetcted variable name");

    Name name = GetTokenName(&token[*tp]);
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

    if (!IsTokenNum(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected number.");

    Number val = GetTokenNumber(&token[*tp]);
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

    if (!IsTokenType(&token[*tp]))
        SYNTAX_ERR_FOR_TOKEN(token[*tp], inputData, "expected type.");

    Type type = GetTokenType(&token[*tp]);
    (*tp)++;

    Node_t* node = {};
    _TYPE(&node, type, nullptr);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number GetTokenNumber(const Token_t* token)
{
    assert(token);
    return token->data.number;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Name GetTokenName(const Token_t* token)
{
    assert(token);
    return token->data.name;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Type GetTokenType(const Token_t* token)
{
    assert(token);
    return token->data.type;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Operation GetTokenOperation(const Token_t* token)
{
    assert(token);
    return token->data.operation;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenEnd(const Token_t* token)
{   
    assert(token);
    return (token->type == TokenType::TokenEndSymbol_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenNum(const Token_t* token)
{
    assert(token);
    return (token->type == TokenType::TokenNumber_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenName(const Token_t* token)
{
    assert(token);
    return (token->type == TokenType::TokenName_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenType(const Token_t* token)
{
    assert(token);
    return (token->type == TokenType::TokenType_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenOperation(const Token_t* token)
{
    assert(token);
    return (token->type == TokenType::TokenOperation_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenFunction(const Token_t* token)
{   
    assert(token);
    return (token->type == TokenType::TokenFunction_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenAssign(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenOperation_t) &&
            (token->data.operation == Operation::assign);   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenSemicolon(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenSeparator_t) &&
            (token->data.separator == Separator::semicolon);   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsBoolOperation(const Token_t* token)
{
    assert(token);
    RETURN_IF_FALSE(token->type == TokenType::TokenOperation_t, false);

    Operation operation = token->data.operation;

    return  (operation == Operation::greater         ) ||
            (operation == Operation::greater_or_equal) ||
            (operation == Operation::less            ) ||
            (operation == Operation::less_or_equal   ) ||
            (operation == Operation::equal           ) ||
            (operation == Operation::not_equal       ) ||
            (operation == Operation::bool_and        ) ||
            (operation == Operation::bool_or         ) ||
            (operation == Operation::bool_not        );            
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsAddSub(const Token_t* token)
{   
    assert(token);
    RETURN_IF_FALSE(token->type == TokenType::TokenOperation_t, false);

    Operation operation = token->data.operation;

    return  (operation == Operation::plus ) || 
            (operation == Operation::minus);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsMulDiv(const Token_t* token)
{
    assert(token);
    RETURN_IF_FALSE(token->type == TokenType::TokenOperation_t, false);

    Operation operation = token->data.operation;
    return  (operation == Operation::mul ) ||
            (operation == Operation::dive);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsPow(const Token_t* token)
{   
    assert(token);
    RETURN_IF_FALSE(token->type == TokenType::TokenOperation_t, false);

    Operation operation = token->data.operation;

    return (operation == Operation::power);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenMinus(const Token_t* token)
{
    assert(token);

    RETURN_IF_FALSE(token->type == TokenType::TokenOperation_t, false);

    Operation operation = token->data.operation;

    return (operation == Operation::minus);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenLeftRoundBracket(const Token_t* token)
{
    assert(token);

    RETURN_IF_FALSE(token->type == TokenType::TokenBracket_t, false);

    Bracket bracket = token->data.bracket;

    return (bracket == Bracket::left_round);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenRightRoundBracket(const Token_t* token)
{
    assert(token);

    RETURN_IF_FALSE(token->type == TokenType::TokenBracket_t, false);

    Bracket bracket = token->data.bracket;

    return (bracket == Bracket::right_round);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenLeftCurlyBracket(const Token_t* token)
{
    assert(token);

    RETURN_IF_FALSE(token->type == TokenType::TokenBracket_t, false);

    Bracket bracket = token->data.bracket;

    return (bracket == Bracket::left_curly);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenRightCurlyBracket(const Token_t* token)
{
    assert(token);

    RETURN_IF_FALSE(token->type == TokenType::TokenBracket_t, false);

    Bracket bracket = token->data.bracket;

    return (bracket == Bracket::right_curly);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsNotAssignOperationBeforeMinus(const Token_t* token, size_t tp)
{
    assert(token);

    return (tp >= 1 && !IsTokenAssign(&token[tp - 1]));   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsOperationToken(const Token_t* token)
{
    assert(token);
    return (token->type == TokenType::TokenOperation_t);   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenConditionIf(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenCondition_t) &&
            (token->data.condition == Condition::if_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenConditionElseIf(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenCondition_t) &&
            (token->data.condition == Condition::else_if_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenConditionElse(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenCondition_t) &&
            (token->data.condition == Condition::else_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenOperationNot(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenOperation_t) &&
            (token->data.operation == Operation::bool_not);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





