
#include <stdio.h>
#include <assert.h>
#include "tree/tree.hpp"
#include "tree/readTree/tokens/token.hpp"
#include "tree/readTree/syntaxErr/syntaxErr.hpp"
#include "tree/readTree/readTreeGlobalnclude.hpp"
#include "tree/readTree/recursiveDescent/recursiveDescent.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#include "log/log.hpp"
#include "tree/treeDump/treeDump.hpp"
#include "tree/readTree/tokens/tokensDump/tokenDump.hpp"
#endif

//------------ Recusrsive Descent function  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t*   GetDefFunc                     (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetDefFuncArgs                 (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetCondition                   (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetIfCondition                 (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetElseIfCondition             (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetElseCondition               (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetCycle                       (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetWhile                       (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetFor                         (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetReturn                      (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetDefVariable                 (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetAssign                      (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetBoolOperation               (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetAddSub                      (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetMulDiv                      (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetBracket                     (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetPow                         (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetCallFunction                (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetCallFunctionArgs            (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetNot                         (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetMinus                       (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetNumber                      (const Token_t* tokensArr, size_t* tp, const InputData* inputData);
static Node_t*   GetName                        (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

static Node_t*   GetType                        (const Token_t* tokensArr, size_t* tp, const InputData* inputData);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Number    GetTokenNumber                  (const Token_t* token);
static Name      GetTokenName                    (const Token_t* token);
static Type      GetTokenType                    (const Token_t* token);
static Operation GetTokenOperation               (const Token_t* token);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool      IsTokenEnd                       (const Token_t* token);
static bool      IsTokenNum                       (const Token_t* token);
static bool      IsTokenName                      (const Token_t* token);
static bool      IsTokenType                      (const Token_t* token);
static bool      IsTokenFunction                  (const Token_t* token);
static bool      IsTokenOperation                 (const Token_t* token);

static bool      IsTokenSeparatorComma            (const Token_t* token);
static bool      IsTokenCycle                     (const Token_t* token);
static bool      IsTokenAssign                    (const Token_t* token);
static bool      IsTokenSemicolon                 (const Token_t* token);
static bool      IsBoolOperation                  (const Token_t* token);
static bool      IsAddSub                         (const Token_t* token);
static bool      IsMulDiv                         (const Token_t* token);
static bool      IsPow                            (const Token_t* token);
static bool      IsTokenLeftRoundBracket          (const Token_t* token);
static bool      IsTokenRightRoundBracket         (const Token_t* token);
static bool      IsTokenMinus                     (const Token_t* token);
static bool      IsTokenConditionIf               (const Token_t* token);
static bool      IsTokenConditionElseIf           (const Token_t* token);
static bool      IsTokenConditionElse             (const Token_t* token);
static bool      IsTokenOperationNot              (const Token_t* token);
static bool      IsTokenLeftCurlyBracket          (const Token_t* token);
static bool      IsTokenRightCurlyBracket         (const Token_t* token);
static bool      IsTokenCycleWhile                (const Token_t* token);
static bool      IsTokenCycleFor                  (const Token_t* token);
static bool      IsTokenFuncAttrCall              (const Token_t* token);
static bool      IsTokenFuncAttrReturn            (const Token_t* token);
static bool      IsTokenOperationSelfChange       (const Token_t* token);
static bool      IsCallFunction                   (const Token_t* tokensArr, const size_t* tp);
static bool      IsNotAssignOperationBeforeMinus  (const Token_t* tokensArr, size_t tp);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Token_t* ConsumeToken                       (const Token_t* tokenArr, size_t* pointer);
const Token_t* PickToken                          (const Token_t* tokenArr, const size_t* pointer);
const Token_t* PickNextToken                      (const Token_t* tokenArr, const size_t* pointer);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Node_t* GetTree(const Token_t* tokensArr, const InputData* inputData)
{
    assert(tokensArr);

    size_t tp = 0;
    Node_t* node = GetDefVariable(tokensArr, &tp, inputData);

    const Token_t* token = ConsumeToken(tokensArr, &tp);
    if (!IsTokenEnd(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '\\0' ");

    TreeErr err = {};

    TREE_ASSERT(NODE_VERIF(node, err));

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetDefFunc(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (IsTokenEnd(token))
        return nullptr;



    Node_t* type_node = GetType(tokensArr, tp, inputData);
    Node_t* name_node = GetName(tokensArr, tp, inputData);

    type_node->left = name_node;

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '('");

    Node_t* args_node = GetDefFuncArgs(tokensArr, tp, inputData);
    
    name_node->left = args_node;
    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '{'");


    Node_t* return_node = GetCondition(tokensArr, tp, inputData);
    name_node->right = return_node;

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '}'");

    Node_t* next_def_func_node = GetDefFunc(tokensArr, tp, inputData);

    // NODE_GRAPHIC_DUMP(next_def_func_node);

    Node_t* def_func_node = {};
    _DEF_FUNC(&def_func_node, type_node);

    Node_t* connect_node = {};
    _CONNECT(&connect_node, def_func_node, next_def_func_node);

    return connect_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetDefFuncArgs(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);
        
    const Token_t* token = PickToken(tokensArr, tp);

    if (!IsTokenType(token))
        return nullptr;
    
    Node_t* type_node = GetType(tokensArr, tp, inputData);

    Node_t* name_node = GetName(tokensArr, tp, inputData);

    type_node->left = name_node;

    token = PickToken(tokensArr, tp);
    if (!IsTokenSeparatorComma(token))
        return type_node;
    (*tp)++;

    Node_t* next_name_node = GetDefFuncArgs(tokensArr, tp, inputData);
    
    Node_t* connect_node = {};
    _CONNECT(&connect_node, type_node, next_name_node);

    return connect_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetCondition(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    Node_t* main_connect_node = {};

    const Token_t* token = PickToken(tokensArr, tp);

    if (IsTokenRightCurlyBracket(token))
        return nullptr;

    if (!IsTokenConditionIf(token))
    {
        Node_t* cycle_node          = GetCycle     (tokensArr, tp, inputData);
        Node_t* next_condition_node = GetCondition (tokensArr, tp, inputData);
        _CONNECT(&main_connect_node, cycle_node, next_condition_node);

        return main_connect_node;
    }

    Node_t* if_node      = GetIfCondition    (tokensArr, tp, inputData);
    Node_t* else_if_node = GetElseIfCondition(tokensArr, tp, inputData);
    Node_t* else_node    = GetElseCondition  (tokensArr, tp, inputData);

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
        Node_t* next_condition = GetCondition(tokensArr, tp, inputData);
        _CONNECT(&connect_node, if_node, next_condition);
        return connect_node;
    }

    Node_t* next_condition = GetCondition(tokensArr, tp, inputData);
    _CONNECT(&main_connect_node, connect_node, next_condition);

    return main_connect_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetIfCondition(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = ConsumeToken(tokensArr, tp);
    if (!IsTokenConditionIf(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected 'if'");

    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected' '(");

    Node_t* if_node = {};

    size_t old_tp = *tp;

    Node_t* bool_node = GetAssign(tokensArr, tp, inputData);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected bool");

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '{'");

    old_tp = *tp;

    Node_t* body_node = GetCondition(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);

    if (!IsTokenRightCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '}'");

    _IF(&if_node, bool_node, body_node);
    
    return if_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetElseIfCondition(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenConditionElseIf(token))
        return nullptr;
    
    (*tp)++;
    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '('");

    

    Node_t* bool_node = GetAssign(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");

    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '{'");

    Node_t* body_node = GetCondition(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '}'");
    
    

    Node_t* else_if_node = {};
    _ELIF(&else_if_node, bool_node, body_node);

    Node_t* next_else_if_node = GetElseIfCondition(tokensArr, tp, inputData);
    
    Node_t* connect_node = {};
    _CONNECT(&connect_node, else_if_node, next_else_if_node);

    return connect_node;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetElseCondition(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenConditionElse(token))
        return nullptr;

    (*tp)++;
    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '{'");

    
    Node_t* body_node = GetCondition(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '}'");


    Node_t* else_node = {};
    _ELSE(&else_node, body_node);

    return else_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetCycle(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (IsTokenCycleFor(token))
        return GetFor(tokensArr, tp, inputData);

    if (IsTokenCycleWhile(token))
        return GetWhile(tokensArr, tp, inputData);

    return GetReturn(tokensArr, tp, inputData);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetWhile(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = ConsumeToken(tokensArr, tp);
    if (!IsTokenCycleWhile(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected 'while'");

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '('");

    
    Node_t* bool_node = GetDefVariable(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");
        
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '{'");
    
    Node_t* condition_node = GetCondition(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '}'");

    Node_t* while_node = {};
    _WHILE(&while_node, bool_node, condition_node);

    return while_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetFor(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = ConsumeToken(tokensArr, tp);
    if (!IsTokenCycleFor(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected 'for'");

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '('");


    Node_t* assign_node_1 = GetDefVariable(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenSemicolon(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ';'");    

    Node_t* bool_node = GetAssign(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenSemicolon(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ';'");


    Node_t* assign_node_2 = GetAssign(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");        
    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '{'");


    Node_t* condition_node = GetCondition(tokensArr, tp, inputData);
    
    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightCurlyBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '}'");


    Node_t* connect_node_1 = {};
    Node_t* connect_node_2 = {};

    _CONNECT(&connect_node_1, assign_node_1, bool_node);
    _CONNECT(&connect_node_2, connect_node_1, assign_node_2);


    Node_t* for_node = {};
    _FOR(&for_node, connect_node_2, condition_node);

    return for_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetReturn(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    Node_t* return_node = {};
    const Token_t* token = PickToken(tokensArr, tp);

    if (!IsTokenFuncAttrReturn(token))
        return_node = GetDefVariable(tokensArr, tp, inputData);
    
    else
    {
        (*tp)++;
        Node_t* bool_operation_node = GetBoolOperation(tokensArr, tp, inputData);
        _RET(&return_node, bool_operation_node);
    }

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenSemicolon(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ';'");

    return return_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetDefVariable(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    
    const Token_t* token = PickToken(tokensArr, tp);
    
    if (!IsTokenType(token))
        return GetAssign(tokensArr, tp, inputData);
    
    Node_t* type_node = GetType(tokensArr, tp, inputData);    
    Node_t* name_node = GetName(tokensArr, tp, inputData);

    type_node->left = name_node;

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenAssign(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '='");

    Node_t* bool_operation_node = GetBoolOperation(tokensArr, tp, inputData);

    Node_t* assign_node = {};

    _ASG(&assign_node, bool_operation_node);

    name_node->left = assign_node;

    Node_t* def_variable_node = {};
    _DEF_VAR(&def_variable_node, type_node);

    return def_variable_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetAssign(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    const Token_t* token      = PickToken(tokensArr, tp);
    const Token_t* next_token = PickNextToken(tokensArr, tp);

    if (!(IsTokenName(token) && IsTokenAssign(next_token)))
        return GetBoolOperation(tokensArr, tp, inputData);
    
    Node_t* name_node = GetName(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenAssign(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '='");

    Node_t* bool_operation_node = GetBoolOperation(tokensArr, tp, inputData);

   
    Node_t* assign_node = {};
    _ASG(&assign_node, bool_operation_node);

    name_node->left = assign_node;

    Node_t* asg_variable_node = {};
    _ASG_VAR(&asg_variable_node, name_node);

    return asg_variable_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetBoolOperation(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    Node_t* node = GetAddSub(tokensArr, tp, inputData);

    const Token_t* token = PickToken(tokensArr, tp);

    while (IsBoolOperation(token))
    {
        Operation operation = GetTokenOperation(token);
        (*tp)++;
        
        Node_t* node2 = GetAddSub(tokensArr, tp, inputData);
    
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
    
            case Operation::bool_not:
            case Operation::plus:
            case Operation::minus:
            case Operation::mul:
            case Operation::dive:
            case Operation::power:
            case Operation::assign: break;
            case Operation::undefined_operation:
            default:
            {
                assert(0 && "you forgot about some bool operation in get booo operation");
                break;
            }
        }

        TREE_ASSERT(SwapNode(&new_node, &node));

        token = PickToken(tokensArr, tp);
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetAddSub(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    size_t old_tp = *tp;

    Node_t* node = GetMulDiv(tokensArr, tp, inputData);

    const Token_t* token = PickToken(tokensArr, tp);

    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected math expression");
    
    
    while(IsAddSub(token))
    {
        Operation operation = GetTokenOperation(token);
        (*tp)++;

        Node_t* node2 = GetMulDiv(tokensArr, tp, inputData);
        Node_t* new_node = {};

        if (operation == Operation::plus)
            _ADD(&new_node, node, node2);
    
        else if (operation == Operation::minus)
            _SUB(&new_node, node, node2);
    
        else
        {
            assert(0 && "incorrect operation type");
        }

        TREE_ASSERT(SwapNode(&node, &new_node));
        token = PickToken(tokensArr, tp);
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetMulDiv(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    size_t old_tp = *tp;

    Node_t* node = GetPow(tokensArr, tp, inputData);

    const Token_t* token = PickToken(tokensArr, tp);
    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected math expression");

    while (IsMulDiv(token))
    {
        Operation operation = GetTokenOperation(token);
        (*tp)++;

        Node_t* node2 = GetPow(tokensArr, tp, inputData);
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
        token = PickToken(tokensArr, tp);
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetPow(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    Node_t* node = GetCallFunction(tokensArr, tp, inputData);

    const Token_t* token = PickToken(tokensArr, tp);
    while(IsPow(token))
    {
        Node_t* node2 = GetCallFunction(tokensArr, tp, inputData);
        (*tp)++;

        Node_t* new_node = {};
        _POW(&new_node, node, node2);
        TREE_ASSERT(SwapNode(&node, &new_node));
        token = PickToken(tokensArr, tp);
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetCallFunction(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);

    if (!IsCallFunction(tokensArr, tp))
        return GetMinus(tokensArr, tp, inputData);

    Node_t* name_node = GetName(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected '('");

    Node_t* args_node = GetCallFunctionArgs(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");

    name_node->left = args_node;

    Node_t* call_func_node = {};
    _CALL_FUNC(&call_func_node, name_node);

    return call_func_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetCallFunctionArgs(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);
    
    const Token_t* token = PickToken(tokensArr, tp);

    if (IsTokenRightRoundBracket(token))
        return nullptr;

    Node_t* bool_operation_node = GetBoolOperation(tokensArr, tp, inputData);    
    

    token = PickToken(tokensArr, tp);
    if (!IsTokenSeparatorComma(token))
        return bool_operation_node;

    (*tp)++;

    size_t old_tp = *tp;
    Node_t* next_name_node = GetCallFunctionArgs(tokensArr, tp, inputData);
    
    token = PickToken(tokensArr, tp);
    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected some expression");

    if (!next_name_node)
        return bool_operation_node;

    Node_t* connect_node = {};
    _CONNECT(&connect_node, bool_operation_node, next_name_node);

    return connect_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetMinus(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenMinus(token))
        return GetNot(tokensArr, tp, inputData);

    (*tp)++;

    token = ConsumeToken(tokensArr, tp);
    if (IsNotAssignOperationBeforeMinus(tokensArr, *tp))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "Operation before '-'");
    
    size_t old_tp = *tp;

    Node_t* node = GetMulDiv(tokensArr, tp, inputData);

    token = PickToken(tokensArr, tp);
    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "Nothig after '-'");

    Node_t* new_node = {};
    _SUB(&new_node, node, nullptr);

    return new_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetNot(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenOperationNot(token))
        return GetBracket(tokensArr, tp, inputData);

    size_t old_tp = *tp;

    (*tp)++;

    Node_t* node = GetMulDiv(tokensArr, tp, inputData);

    token = PickToken(tokensArr, tp);
    if (old_tp == *tp)
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "nothing after '!'");

    Node_t* not_node = {};
    _NOT(&not_node, node);

    return not_node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetBracket(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenLeftRoundBracket(token))
        return GetNumber(tokensArr, tp, inputData);

    (*tp)++;

    Node_t* node = GetBoolOperation(tokensArr, tp, inputData);

    token = ConsumeToken(tokensArr, tp);
    if (!IsTokenRightRoundBracket(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected ')'");

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetNumber(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenNum(token))
        return GetName(tokensArr, tp, inputData);
    
    Number val = GetTokenNumber(token);
    (*tp)++;

    Node_t* node = {};
    _NUM(&node, val);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetName(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tp);
    assert(tokensArr);

    const Token_t* token = PickToken(tokensArr, tp);

    if (!IsTokenName(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected some expression");

    Name name = GetTokenName(token);
    (*tp)++;

    Node_t* node = {};
    _NAME(&node, name);

    return node;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node_t* GetType(const Token_t* tokensArr, size_t* tp, const InputData* inputData)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token = PickToken(tokensArr, tp);
    if (!IsTokenType(token))
        SYNTAX_ERR_FOR_TOKEN(token, inputData, "expected type.");

    Type type = GetTokenType(token);
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

static bool IsTokenOperation(const Token_t* token)
{   
    assert(token);
    return token->type == TokenType::TokenOperation_t;    
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

static bool IsNotAssignOperationBeforeMinus(const Token_t* tokensArr, size_t tp)
{
    assert(tokensArr);

    return (tp >= 1 && IsTokenOperation(&tokensArr[tp - 1]) && !IsTokenAssign(&tokensArr[tp - 1]));   
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

static bool IsTokenCycle(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenCycle_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenCycleWhile(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenCycle_t) &&
            (token->data.cycle == Cycle::while_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenCycleFor(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenCycle_t) &&
            (token->data.cycle == Cycle::for_t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenSeparatorComma(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenSeparator_t) &&
            (token->data.separator == Separator::comma);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenFuncAttrCall(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenFuncAttr_t) &&
            (token->data.attribute == FunctionAttribute::call);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenFuncAttrReturn(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenFuncAttr_t) &&
            (token->data.attribute == FunctionAttribute::ret);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsCallFunction(const Token_t* tokensArr, const size_t* tp)
{
    assert(tokensArr);
    assert(tp);

    const Token_t* token     = &tokensArr[*tp];
    const Token_t* next_token = &tokensArr[*tp + 1];

    return  IsTokenName            (token     ) && 
            IsTokenLeftRoundBracket(next_token);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsTokenOperationSelfChange(const Token_t* token)
{
    assert(token);
    return  (token->type == TokenType::TokenOperation_t) && 
            (
            (token->data.operation == Operation::plus_equal ) ||
            (token->data.operation == Operation::minus_equal) ||
            (token->data.operation == Operation::mul_equal  ) ||
            (token->data.operation == Operation::div_equal  ) ||
            (token->data.operation == Operation::plus_plus  ) ||
            (token->data.operation == Operation::plus_plus  ) ||
            (token->data.operation == Operation::plus_equal ) ||
            (token->data.operation == Operation::plus_equal )
            );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Token_t* ConsumeToken(const Token_t* tokenArr, size_t* pointer)
{
    assert(tokenArr);
    assert(pointer);

    size_t pointer_copy = *pointer;
    (*pointer)++;

    return &tokenArr[pointer_copy];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Token_t* PickToken(const Token_t* tokenArr, const size_t* pointer)
{
    assert(tokenArr);
    assert(pointer);

    return &tokenArr[*pointer];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Token_t* PickNextToken(const Token_t* tokenArr, const size_t* pointer)
{
    assert(tokenArr);
    assert(pointer);

    return &tokenArr[*pointer + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
