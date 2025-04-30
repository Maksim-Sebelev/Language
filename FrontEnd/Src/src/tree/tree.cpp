#include <assert.h>
#include "tree/tree.hpp"
#include "tree/treeDump/treeDump.hpp"
#include "tree/readTree/fileread/fileread.hpp"
#include "tree/readTree/recursiveDescent/recursiveDescent.hpp"
#include "tree/readTree/tokens/token.hpp"
#include "lib/colorPrint.hpp"
#include "lib/lib.hpp"
#include "tree/nameTable/nametable.hpp"
#include "tree/readTree/tokens/tokensDump/tokenDump.hpp"


//======================================================================================================================================================================

static bool IsError                        (const TreeErr* err);
static bool HasntNumChild                  (const Node_t* node);
static bool HasOperationChildren           (const Node_t* node);
static bool IsNodeTypeOperationDataCorrect (const Node_t* node);

//======================================================================================================================================================================

static void    PrintError                  (const TreeErr* err);
static TreeErr AllNodeVerif                (const Node_t* node, size_t* treeSize);

//============================== Tree functions ============================================================================================================================

TreeErr TreeCtor(Tree_t* tree, const char* inputFile)
{
    TreeErr err = {};

    size_t    inputLen  = 0;
    InputData inputData = ReadFile(inputFile, &inputLen);

    tree->inputData = inputData;

    size_t   tokenQuant = 0;
    Token_t* token      = ReadInputStr(&inputData, inputLen, &tokenQuant);

    if (!token)
    {
        COLOR_PRINT(RED, "\nEmpty input file.\n\n");
        exit(0); 
    }

    TOKEN_GRAPHIC_DUMP  (token, tokenQuant);
    TOKENS_LOG(token, tokenQuant, &inputData);
    tree->root = GetTree(token, &inputData);

    // NAME_TABLE_GRAPHIC_DUMP(&tree->nameTable);

    TokenDtor(token);

    return TREE_VERIF(tree, err);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr TreeDtor(Tree_t* tree)
{
    assert(tree);
    assert(tree->root);

    TreeErr Err = {};

    TREE_ASSERT(NodeAndUnderTreeDtor(tree->root));
    tree->root = nullptr;

    InputDataDtor(&tree->inputData);

    return TREE_VERIF(tree, Err);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr NodeAndUnderTreeDtor(Node_t* node)
{
    TreeErr err = {};

    if (node == nullptr) return err;

    if (node->left ) NodeAndUnderTreeDtor(node->left );
    if (node->right) NodeAndUnderTreeDtor(node->right);

    NodeDtor(node);

    return err;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr NodeCtor(Node_t** node, NodeArgType type, NodeData_t data, Node_t* left, Node_t* right)
{
    TreeErr err = {};

    *node = (Node_t*) calloc(1, sizeof(Node_t));

    if (*node == NULL)
    {
        err.err = TreeErrorType::CTOR_CALLOC_RETURN_NULL;
        return NODE_VERIF(*node, err);
    }

    (*node)->type = type;

    (*node)->data = data;

    (*node)->left  = left;
    (*node)->right = right;

    return NODE_VERIF(*node, err);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr NodeDtor(Node_t* node)
{
    assert(node);

    TreeErr err = {};

    node->left  = nullptr;
    node->right = nullptr;

    FREE(node);

    CodePlaceCtor(&err.place, __FILE__, __LINE__, __func__);
    return err;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr NodeCopy(Node_t** copy, const Node_t* node)
{
    assert(node);

    TreeErr err = {};

    NodeArgType type  = node->type;
    NodeData_t  data  = node->data;
    Node_t*     left  = node->left;
    Node_t*     right = node->right;

    TREE_ASSERT(NodeCtor(copy, type, data, left, right));


    if (*copy == nullptr)
    {
        err.err = TreeErrorType::NODE_NULL;
        return NODE_VERIF(*copy, err);
    }

    if (node->left)
    {
        TREE_ASSERT(NodeCopy(&(*copy)->left,  node->left));
    }

    if (node->right)
    {
        TREE_ASSERT(NodeCopy(&(*copy)->right, node->right));
    }

    return NODE_VERIF(*copy, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr NodeSetCopy(Node_t* copy, const Node_t* node)
{
    assert(node);

    TreeErr err = {};

    NodeArgType type  = node->type;
    
    Node_t*     left  = node->left;
    Node_t*     right = node->right;

    switch (type)
    {
        case NodeArgType::number:
        {
            Number number = node->data.num;
            _SET_NUM(copy, number);
            break;
        }
        case NodeArgType::name:  
        {
            Name name = node->data.name;
            _SET_NAME(copy, name);
            break;
        }
        case NodeArgType::operation: 
        {
            Operation operation = node->data.oper;
            _SET_OPER(copy, operation, left, right);
            break;
        }
        case NodeArgType::condition:
        {
            Condition condition = node->data.condition;
            _SET_COND(copy, condition, left, right);
            break;
        }
        case NodeArgType::cycle:
        {
            Cycle cycle = node->data.cycle;
            _SET_CYCLE(copy, cycle, left, right);
            break;
        }
        case NodeArgType::type:
        {
            Type typ = node->data.type;
            _SET_TYPE(copy, typ, left);
            break;
        }
        case NodeArgType::mainInfo:
        {
            MainStartEnd main = node->data.mainInfo;
            _SET_MAIN(copy, main, left);
            break;
        }
        // case NodeArgType::function:
        // {
            // DFunction function = node->data.func;
            // _SET_FUNC(copy, function, left);
            // break;
            // }
        case NodeArgType::connect:
        case NodeArgType::undefined:
        default:  err.err = TreeErrorType::NODE_NULL; return NODE_VERIF(node, err);
    }

    return NODE_VERIF(copy, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr SetNode(Node_t* node, NodeArgType type, NodeData_t data, Node_t* left, Node_t* right)
{
    assert(node);

    TreeErr err = {};
    NODE_RETURN_IF_ERR(node, err);

    node->left  = left;
    node->right = right;

    node->type = type;

    node->data = data;

    return NODE_VERIF(node, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr SwapNode(Node_t** node1, Node_t** node2)
{
    assert(node1);
    assert(node2);

    TreeErr err = {};

    Node_t* temp = *node1;
    *node1 = *node2;
    *node2 = temp;

    err = NODE_VERIF(*node1, err);
    if (IsError(&err)) return err;
    return NODE_VERIF(*node2, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr TreeVerif(const Tree_t* tree, TreeErr* err, const char* file, const int line, const char* func)
{

    assert(err);
    assert(file);
    assert(func);

    CodePlaceCtor(&err->place, file, line, func);

    RETURN_IF_FALSE(tree->root, *err);

    RETURN_IF_TRUE(IsError(err), *err);

    size_t treeSize = 0;
    *err = AllNodeVerif(tree->root, &treeSize);

    RETURN_IF_TRUE(IsError(err), *err);
    // RETURN_IF_FALSE(treeSize == tree->size, *err, err->err = TreeErrorType::INCORRECT_TREE_SIZE);

    return *err;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr NodeVerif(const Node_t* node, TreeErr* err, const char* file, const int line, const char* func)
{
    assert(node);
    assert(err);
    assert(file);
    assert(func);

    CodePlaceCtor(&err->place, file, line, func);

    RETURN_IF_FALSE(node, *err);

    NodeArgType type = node->type;

    switch (type)
    {
        case NodeArgType::number:
        {
            RETURN_IF_FALSE(HasntNumChild(node), *err, err->err = TreeErrorType::NUM_HAS_INCORRECT_CHILD_QUANT);
            break;
        }

        case NodeArgType::name:
        {
            // RETURN_IF_FALSE(IsNodeVariableTypeUndef(node), *err, err->err = TreeErrorType::VAR_TYPE_NODES_ARG_IS_UNDEFINED);
            // RETURN_IF_FALSE(HasntVarChild          (node), *err, err->err = TreeErrorType::VAR_HAS_INCORRECT_CHILD_QUANT);
            break;
        }

        case NodeArgType::operation:
        {
            RETURN_IF_FALSE(IsNodeTypeOperationDataCorrect (node), *err, err->err = TreeErrorType::OPER_TYPE_NODES_ARG_IS_UNDEFINED);
            RETURN_IF_FALSE(HasOperationChildren           (node), *err, err->err = TreeErrorType::OPER_HAS_INCORRECT_CHILD_QUANT);
            break;
        }

        case NodeArgType::type:
        {
            return *err;
        }

        case NodeArgType::connect:
        {
            return *err;
        }

        case NodeArgType::condition:
        {
            return *err;
        }

        case NodeArgType::cycle:
        {
            return *err;
        }

        case NodeArgType::mainInfo:
        {
            return *err;
        }

        case NodeArgType::main:
        {
            return *err;
        }

        case NodeArgType::attribute:
        {
            return *err;
        }

        case NodeArgType::undefined:
        default:
        {
            err->err = TreeErrorType::UNDEFINED_NODE_TYPE;
            return *err;
        }
    }

    return *err;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool HasntNumChild(const Node_t* node)
{
    assert(node);
    assert(node->type == NodeArgType::number);

    return !(node->left) && !(node->right);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool HasOperationChildren(const Node_t* node)
{
    assert(node);
    assert(node->type == NodeArgType::operation);

    Operation operation = node->data.oper;

    if (operation == Operation::minus || operation == Operation::bool_not)
    {
        return (node->left);
    }

    return (node->left) && (node->right);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsError(const TreeErr* err)
{
    assert(err);
    return err->err != TreeErrorType::NO_ERR;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsNodeTypeOperationDataCorrect(const Node_t* node)
{
    assert(node);
    assert(node->type == NodeArgType::operation);

    Operation operation = node->data.oper;

    return (operation != Operation::undefined_operation);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static bool IsNodeTypeDFunctionDataCorrect(const Node_t* node)
// {
//     assert(node);
//     assert(node->type == NodeArgType::function);

//     DFunction fucntion = node->data.func;

//     switch (fucntion)
//     {
//         case DFunction::Sqrt:
//         case DFunction::Ln:
//         case DFunction::Sin:
//         case DFunction::Cos:
//         case DFunction::Tg:
//         case DFunction::Ctg:
//         case DFunction::Sh:
//         case DFunction::Ch:
//         case DFunction::Th:
//         case DFunction::Cth:
//         case DFunction::Arcsin:
//         case DFunction::Arccos:
//         case DFunction::Arctg:
//         case DFunction::Arcctg: break;
//         case DFunction::undefined_function:
//         default: return false;
//     }

//     return true;
// }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static bool IsNodeVariableTypeUndef(const Node_t* node)
// {
//     assert(node);
//     assert(node->type == NodeArgType::variable);

//     Variable variable = node->data.var;

//     switch (variable)
//     {
//         case Variable::x:
//         case Variable::y: break;
//         case Variable::undefined_variable:
//         default: return false;
//     }

//     return true;
// }

// //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static TreeErr AllNodeVerif(const Node_t* node, size_t* treeSize)
{
    assert(node);
    assert(treeSize);

    TreeErr err = {};

    TREE_ASSERT(NODE_VERIF(node, err));

    if (node->left)
    {
        (*treeSize)++;
        err = AllNodeVerif(node->left, treeSize);
        RETURN_IF_TRUE(IsError(&err), err);
    }

    if (node->right)
    {
        (*treeSize)++;
        err = AllNodeVerif(node->right, treeSize);
        RETURN_IF_TRUE(IsError(&err), err);
    }

    return err;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintError(const TreeErr* Err)
{
    assert(Err);
    TreeErrorType err = Err->err;

    switch (err)
    {
        case TreeErrorType::NO_ERR:
            return;
            break;
    
        case TreeErrorType::FAILED_OPEN_INPUT_STREAM:
            COLOR_PRINT(RED, "Error: failed to open input file.\n");
            break;

        case TreeErrorType::NODE_NULL:
            COLOR_PRINT(RED, "Error: node is nullptr and now this is bad.\n");
            break;
        
        case TreeErrorType::CTOR_CALLOC_RETURN_NULL:
            COLOR_PRINT(RED, "Error: failed alocate memory in ctor.\n");
            break;

        case TreeErrorType::INSERT_INCORRECT_SITUATION:
            COLOR_PRINT(RED, "Error: undefined situation in insert.\n");
            break;

        case TreeErrorType::DTOR_NODE_WITH_CHILDREN:
            COLOR_PRINT(RED, "Error: Dtor node that childern has.\n");
            break;
        
        case TreeErrorType::INCORRECT_TREE_SIZE:
            COLOR_PRINT(RED, "Error: Incorrect tree size.\n");
            break;

        case TreeErrorType::NUM_TYPE_NODES_ARG_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'number' type, but arg is undefined.\n");
            break;

        case TreeErrorType::NUM_HAS_INCORRECT_CHILD_QUANT:
            COLOR_PRINT(RED, "Error: Node has 'number' type, but child quant is incorrect.\n");
            break;
        
        case TreeErrorType::VAR_TYPE_NODES_ARG_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'variable' type, but arg is undefined.\n");
            break;

        case TreeErrorType::VAR_HAS_INCORRECT_CHILD_QUANT:
            COLOR_PRINT(RED, "Error: Node has 'varible' type, but child quant is incorrect.\n");
            break;

        case TreeErrorType::OPER_TYPE_NODES_ARG_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'operation' type, but arg is undefined.\n");
            break;

        case TreeErrorType::OPER_HAS_INCORRECT_CHILD_QUANT:
            COLOR_PRINT(RED, "Error: Node has 'operation' type, but child quant is incorrect.\n");
            break;

        case TreeErrorType::FUNC_TYPE_NODES_ARG_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'function' type, but arg is undefined type.\n");
            break;

        case TreeErrorType::FUNC_HAS_INCORRECT_CHILD_QUANT:
            COLOR_PRINT(RED, "Error: Node has 'function' type, but child quant is incorrect.\n");
            break;

        case TreeErrorType::UNDEFINED_NODE_TYPE:
            COLOR_PRINT(RED, "Error: Node has undefined type.\n");
            break;

        case TreeErrorType::NODE_IS_NUM_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'number' type, but not only her 'number' type is defined.\n");
            break;

        case TreeErrorType::NODE_IS_OPERATION_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'operation' type, but not only her 'operation' type is defined.\n");
            break;

        case TreeErrorType::NODE_IS_FUNCTION_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED:
            COLOR_PRINT(RED, "Error: Node has 'function' type, but not only her 'function' type is defined.\n");
            break;

        // case TreeErrorType::NODE_IS_VARIABLE_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED:
        //     COLOR_PRINT(RED, "Error: Node has 'variable' type, but not only her 'variable' type is defined.\n");
        //     break;
        
        case TreeErrorType::UNDEFINED_FUNCTION_TYPE:
            COLOR_PRINT(RED, "Error: Node has 'function' type, bit it is undefined.\n");
            break;

        case TreeErrorType::UNDEFINED_OPERATION_TYPE:
            COLOR_PRINT(RED, "Error: Node has 'operation' type, bit it is undefined.\n");
            break;

        case TreeErrorType::DIVISION_BY_0:
            COLOR_PRINT(RED, "Error: division by 0.\n");
            break;

        default:
            assert(0 && "you forgot about some error in print error.\n");
            return;
            break;
    }

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TreeAssertPrint(TreeErr* err, const char* file, const int line, const char* func)
{
    assert(err);
    assert(file);
    assert(func);
    COLOR_PRINT(RED, "\nAssert made in:\n");
    PrintPlace(file, line, func);
    PrintError(err);
    PrintPlace(err->place.file, err->place.line, err->place.func);
    printf("\n");
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------