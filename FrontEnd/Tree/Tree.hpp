#ifndef TREE_HPP
#define TREE_HPP

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "../Common/ColorPrint.hpp"
#include "../Common/GlobalInclude.hpp"
#include "NameTable/NameTable.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum class TreeErrorType
{
    NO_ERR,
    FAILED_OPEN_INPUT_STREAM,
    CTOR_CALLOC_RETURN_NULL,
    INSERT_INCORRECT_SITUATION,
    DTOR_NODE_WITH_CHILDREN,
    NUM_TYPE_NODES_ARG_IS_UNDEFINED,
    VAR_TYPE_NODES_ARG_IS_UNDEFINED,
    OPER_TYPE_NODES_ARG_IS_UNDEFINED,
    FUNC_TYPE_NODES_ARG_IS_UNDEFINED,
    NODE_IS_NUM_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED,
    NODE_IS_OPERATION_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED,
    NODE_IS_FUNCTION_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED,
    NODE_IS_NAMEIABLE_TYPE_BUT_OTHER_NODE_TYPED_IS_UNDEFINED,
    OPER_HAS_INCORRECT_CHILD_QUANT,
    NUM_HAS_INCORRECT_CHILD_QUANT,
    VAR_HAS_INCORRECT_CHILD_QUANT,
    FUNC_HAS_INCORRECT_CHILD_QUANT,
    UNDEFINED_NODE_TYPE,
    UNDEFINED_OPERATION_TYPE,
    UNDEFINED_FUNCTION_TYPE,
    INCORRECT_TREE_SIZE,
    DIVISION_BY_0,
    NODE_NULL,
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct TreeErr
{
    TreeErrorType err;
    CodePlace place;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum class NodeArgType
{
    undefined,
    operation,
    name,
    number,
    function,
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum class Operation
{
    undefined_operation,
    plus  , 
    minus ,
    mul   , 
    dive  ,
    power ,
    assign,
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum class Function
{
    undefined_function,
    Sqrt,
    Ln,
    Sin,
    Cos,
    Tg,
    Ctg,
    Arcsin,
    Arccos,
    Arctg,
    Arcctg,
    Sh,
    Ch,
    Th,
    Cth,
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef double Number;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef size_t NamePointer;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

union NodeData_t
{
    Operation    oper;
    Number       num;
    Function     func;
    NamePointer  name;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Node_t
{
    NodeArgType type;
    NodeData_t  data;
    Node_t*     right;
    Node_t*     left;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Tree_t
{
    Node_t*     root;
    NameTable_t nameTable;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TreeErr TreeCtor               (Tree_t*  tree, const char* input, char** s);
TreeErr TreeDtor               (Tree_t*  root);
TreeErr NodeCtor               (Node_t** node, NodeArgType type, NodeData_t data, Node_t* left, Node_t* right);
TreeErr NodeDtor               (Node_t*  node);
TreeErr NodeAndUnderTreeDtor   (Node_t*  node);

TreeErr NodeCopy               (Node_t** copy, const Node_t* node);
TreeErr NodeSetCopy            (Node_t*  copy, const Node_t* node);
TreeErr SetNode                (Node_t*  node, NodeArgType type, NodeData_t data, Node_t* left, Node_t* right);
TreeErr SwapNode               (Node_t** node1, Node_t** node2);

TreeErr TreeVerif              (const Tree_t* tree, TreeErr* Err, const char* file, const int line, const char* func);
TreeErr NodeVerif              (const Node_t* node, TreeErr* err, const char* file, const int line, const char* func);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define _NUM(  node, val                   ) do { NodeData_t data = {.num  = val};                 TREE_ASSERT(NodeCtor(node, NodeArgType::number,    data,  nullptr,      nullptr)); }       while(0)
#define _FUNC( node, val, left             ) do { NodeData_t data = {.func = val};                 TREE_ASSERT(NodeCtor(node, NodeArgType::function,  data,  left,         nullptr)); }       while(0)
#define _NAME( node, val                   ) do { NodeData_t data = {.name = val};                 TREE_ASSERT(NodeCtor(node, NodeArgType::name,      data,  nullptr,      nullptr)); }       while(0)
#define _OPER( node, val, left, right      ) do { NodeData_t data = {.oper = val};                 TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data,  left,         right));   }       while(0)

#define _SET_NUM(  node, val               ) do { NodeData_t data = {.num  = val};                 TREE_ASSERT(SetNode (node, NodeArgType::number,    data, nullptr,       nullptr)); }       while(0)
#define _SET_FUNC( node, val, left         ) do { NodeData_t data = {.func = val};                 TREE_ASSERT(SetNode (node, NodeArgType::function,  data, left,          nullptr)); }       while(0)
#define _SET_NAME( node, val               ) do { NodeData_t data = {.name = val};                 TREE_ASSERT(SetNode (node, NodeArgType::name,      data, nullptr,       nullptr)); }       while(0)
#define _SET_OPER( node, val, left, right  ) do { NodeData_t data = {.oper = val};                 TREE_ASSERT(SetNode (node, NodeArgType::operation, data, left,          right));   }       while(0)

#define _SET_FUNC_ONLY( node, val          ) do { NodeData_t data = {.func = val};                 TREE_ASSERT(SetNode (node, NodeArgType::function,  data, (node)->left,  (node)->right)); } while(0)
#define _SET_OPER_ONLY( node, val          ) do { NodeData_t data = {.oper = val};                 TREE_ASSERT(SetNode (node, NodeArgType::operation, data, (node)->left,  (node)->right)); } while(0)

#define _MUL( node, left, right            ) do { NodeData_t data = {.oper = Operation::mul};      TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data, left,          right)); }         while(0)
#define _DIV( node, left, right            ) do { NodeData_t data = {.oper = Operation::dive};     TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data, left,          right)); }         while(0)
#define _ADD( node, left, right            ) do { NodeData_t data = {.oper = Operation::plus};     TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data, left,          right)); }         while(0)
#define _SUB( node, left, right            ) do { NodeData_t data = {.oper = Operation::minus};    TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data, left,          right)); }         while(0)
#define _POW( node, left, right            ) do { NodeData_t data = {.oper = Operation::power};    TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data, left,          right)); }         while(0)
#define _ASG( node, left, right            ) do { NodeData_t data = {.oper = Operation::assign};   TREE_ASSERT(NodeCtor(node, NodeArgType::operation, data, left,          right)); }         while(0)


#define _SET_MUL( node, left, right        ) do { NodeData_t data = {.oper = Operation::mul};      TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, left,         right)); }         while(0)
#define _SET_DIV( node, left, right        ) do { NodeData_t data = {.oper = Operation::dive};     TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, left,         right)); }         while(0)
#define _SET_ADD( node, left, right        ) do { NodeData_t data = {.oper = Operation::plus};     TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, left,         right)); }         while(0)
#define _SET_SUB( node, left, right        ) do { NodeData_t data = {.oper = Operation::minus};    TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, left,         right)); }         while(0)
#define _SET_POW( node, left, right        ) do { NodeData_t data = {.oper = Operation::power};    TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, left,         right)); }         while(0)
#define _SET_ASG( node, left, right        ) do { NodeData_t data = {.oper = Operation::assign};   TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, left,         right)); }         while(0)


#define _SET_MUL_ONLY( node                ) do { NodeData_t data = {.oper = Operation::mul};      TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, (node)->left, (node)->right)); } while(0)
#define _SET_DIV_ONLY( node                ) do { NodeData_t data = {.oper = Operation::dive};     TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, (node)->left, (node)->right)); } while(0)
#define _SET_ADD_ONLY( node                ) do { NodeData_t data = {.oper = Operation::plus};     TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, (node)->left, (node)->right)); } while(0)
#define _SET_SUB_ONLY( node                ) do { NodeData_t data = {.oper = Operation::minus};    TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, (node)->left, (node)->right)); } while(0)
#define _SET_POW_ONLY( node                ) do { NodeData_t data = {.oper = Operation::power};    TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, (node)->left, (node)->right)); } while(0)
#define _SET_ASG_ONLY( node                ) do { NodeData_t data = {.oper = Operation::assign};   TREE_ASSERT(SetNode (node, NodeArgType::operation,  data, (node)->left, (node)->right)); } while(0)

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TREE_VERIF(TreePtr, Err) TreeVerif(TreePtr, &Err, __FILE__, __LINE__, __func__)

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NODE_VERIF(Node, Err)    NodeVerif(Node,    &Err, __FILE__, __LINE__, __func__)

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TREE_RETURN_IF_ERR(TreePtr, Err) do                          \
{                                                                     \
    TreeErr ErrCopy = Err;                                             \
    TreeVerif(TreePtr, &ErrCopy, __FILE__, __LINE__, __func__);         \
    if (ErrCopy.err != NO_ERR)                                           \
    {                                                                     \
        return ErrCopy;                                                    \
    }                                                                       \
} while (0)                                                                  \

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NODE_RETURN_IF_ERR(Node, Err) do                             \
{                                                                     \
    TreeErr ErrCopy = Err;                                             \
    NodeVerif(Node, &ErrCopy, __FILE__, __LINE__, __func__);            \
    if (ErrCopy.err != TreeErrorType::NO_ERR)                            \
    {                                                                     \
        return ErrCopy;                                                    \
    }                                                                       \
} while (0)                                                                  \


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TREE_ASSERT(Err) do                                          \
{                                                                     \
    TreeErr ErrCopy = Err;                                             \
    if (ErrCopy.err != TreeErrorType::NO_ERR)                           \
    {                                                                    \
        TreeAssertPrint(&ErrCopy, __FILE__, __LINE__, __func__);          \
        COLOR_PRINT(CYAN, "abort() in 3, 2, 1...\n");                      \
        abort();                                                            \
    }                                                                        \
} while (0)                                                                   \

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TreeAssertPrint(TreeErr* Err, const char* File, int Line, const char* Func);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct DefaultFunction
{
    const char* name;
    Function    value;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static DefaultFunction DefaultFunctions[]
{
    {"sqrt"  , Function::Sqrt  },
    {"ln"    , Function::Ln    },
    {"sin"   , Function::Sin   },
    {"cos"   , Function::Cos   },
    {"tg"    , Function::Tg    },
    {"ctg"   , Function::Ctg   },
    {"sh"    , Function::Sh    },
    {"ch"    , Function::Ch    },
    {"th"    , Function::Th    },
    {"cth"   , Function::Cth   },
    {"arcsin", Function::Arcsin},
    {"arccos", Function::Arccos},
    {"arctg" , Function::Arctg },
    {"arcctg", Function::Arcctg},
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const size_t DefaultFunctionsQuant = sizeof(DefaultFunctions) / sizeof(DefaultFunctions[0]);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct DefaultOperation
{
    const char* name;
    Operation   value;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static DefaultOperation DefaultOperations[]
{
    {"+", Operation::plus  },
    {"-", Operation::minus },
    {"*", Operation::mul   },
    {"/", Operation::dive  },
    {"^", Operation::power },
    {"=", Operation::assign},
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const size_t DefaultOperationsQuant = sizeof(DefaultOperations) / sizeof(DefaultOperations[0]);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#endif
