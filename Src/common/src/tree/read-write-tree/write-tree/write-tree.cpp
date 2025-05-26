#include <stdlib.h>
#include <assert.h>
#include "lib/lib.hpp"
#include "tree/tree.hpp"
#include "tree/read-write-tree/write-tree/write-tree.hpp"
#include "tree/read-write-tree/read-write-tree-global/read-write-tree-global.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#include "log/log.hpp"
#include "tree/tree-dump/tree-dump.hpp"
#endif // _DEBUG

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define _TAB

#ifdef _TAB
    #define ON_TAB(...) __VA_ARGS__
    #define OFF_TAB(...)
#else // _TAB
    #define ON_TAB(...)
    #define OFF_TAB(...)  __VA_ARGS__
#endif // _TAB

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintSignature         (FILE* outstream);

static void PrintDefFunc           (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintDefFuncArg        (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintCondition         (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintConditionIf       (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintConditionElseIf   (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintConditionElse     (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintCycle             (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintCycleWhile        (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintCycleFor          (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintReturn            (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintDefVariable       (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintAssign            (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintOperation         (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintCallFunction      (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintCallFunctionArgs  (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintNumber            (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintName              (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintType              (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_TAB
(
static void PrintNTab         (FILE* outstream, size_t nTab                                   );
static void PrintSlashN       (FILE* outstream                                                );
)
static void PrintLeftBracket  (FILE* outstream                     ON_TAB(, size_t nTab      ));
static void PrintRightBracket (FILE* outstream                     ON_TAB(, size_t nTab      ));
static void PrintBefore       (FILE* outstream                     ON_TAB(, size_t nTab      ));
static void PrintAfter        (FILE* outstream                                                );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OFF_TAB
(
static void PrintSpace        (FILE* outstream                                                );
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_WRITE_TREE_DEBUG(
void WherePrintTreeIs(const char* func);
)

#define WHERE_PRINT_TREE_IS() ON_WRITE_TREE_DEBUG(WherePrintTreeIs(__func__))

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintTree(const Tree_t* tree, const char* outstream)
{
    WHERE_PRINT_TREE_IS();

    assert(tree);
    assert(outstream);

    FILE* out = fopen(outstream, "wb");

    if (!out)
        EXIT(EXIT_FAILURE, "failed open '%s'", outstream);

    PrintSignature(out);
    PrintDefFunc  (out, tree->root ON_TAB(, 0));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintSignature(FILE* outstream)
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    fprintf(outstream,  "%s\n"
                        "%s\n"
                        "%s\n"
                        "%s\n"
                        "\n",
                        ast_file_signature,
                        ast_file_signature_name,
                        ast_file_signature_autor,
                        ast_file_signature_version
                    );

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintDefFunc(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;
    
    if (node->type == NodeArgType::connect)
    {
        PrintDefFunc(outstream, node->left  ON_TAB(, nTabBefore));
        PrintDefFunc(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (node->type != NodeArgType::initialisation || node->data.init != Initialisation::def_function)
        EXIT(EXIT_FAILURE, "here must be def func node");

    const Node_t* type_node = node->left;
    const Node_t* name_node = node->left->left;
    const Node_t* args_node = node->left->left->left;
    const Node_t* body_node = node->left->left->right;

    PrintBefore       (outstream              ON_TAB(, nTabBefore    ));
    fprintf           (outstream, "%s",       def_func                );
    PrintAfter        (outstream                                      );
    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore    ));

    PrintType         (outstream, type_node   ON_TAB(, nTabBefore + 1));
    PrintName         (outstream, name_node   ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                     ));

    PrintBefore       (outstream              ON_TAB(, nTabBefore + 1));
    fprintf           (outstream, "%s",       arguments               );
    PrintAfter        (outstream                                      );
    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore + 1));
    PrintDefFuncArg   (outstream, args_node   ON_TAB(, nTabBefore + 2));
    PrintRightBracket (outstream              ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                     ));
    PrintBefore       (outstream              ON_TAB(, nTabBefore + 1));
    fprintf           (outstream, "%s",       body                    );
    PrintAfter        (outstream                                      );
    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore + 1));
    PrintCondition    (outstream, body_node   ON_TAB(, nTabBefore + 2));
    PrintRightBracket (outstream              ON_TAB(, nTabBefore + 1));

    PrintRightBracket (outstream              ON_TAB(, nTabBefore    ));

    ON_TAB(PrintSlashN(outstream                                     ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintDefFuncArg(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    PrintType(outstream, node->left       ON_TAB(, nTabBefore));
    PrintName(outstream, node->left->left ON_TAB(, nTabBefore));
    
    if (!node->right) return;
    
    ON_TAB(
    PrintSlashN(outstream);
    )

    if (node->right->type == NodeArgType::connect)
        return PrintDefFuncArg(outstream, node->right ON_TAB(, nTabBefore));

    PrintType(outstream, node->right       ON_TAB(, nTabBefore));
    PrintName(outstream, node->right->left ON_TAB(, nTabBefore));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintCondition(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (node->type == NodeArgType::connect)
    {
        PrintCondition(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCondition(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    NodeData_t data = node->data;

    if (type == NodeArgType::condition && data.condition == Condition::if_t)
        return PrintConditionIf(outstream, node ON_TAB(, nTabBefore));

    else if (type == NodeArgType::condition && data.condition == Condition::else_if_t)
        return PrintConditionElseIf(outstream, node ON_TAB(, nTabBefore));

    else if (type == NodeArgType::condition && data.condition == Condition::else_t)
    {
        return PrintConditionElse(outstream, node ON_TAB(, nTabBefore));
    }

    return PrintCycle(outstream, node ON_TAB(, nTabBefore));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintConditionIf(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node || node->type != NodeArgType::condition || node->data.condition != Condition::if_t)
        EXIT(EXIT_FAILURE, "here must be connect node (left = if; right = else(if))'");

    PrintBefore      (outstream                 ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",          condition_if            );
    PrintAfter       (outstream                                         );
    PrintLeftBracket (outstream                 ON_TAB(, nTabBefore    ));
    
    PrintBefore      (outstream                 ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",          condition               );
    PrintAfter       (outstream                                         );
    PrintLeftBracket (outstream                 ON_TAB(, nTabBefore + 1));
    PrintAssign      (outstream, node->left     ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream                 ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                       ));

    PrintBefore      (outstream                 ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",          body                    );
    PrintAfter       (outstream                                         );
    PrintLeftBracket (outstream                 ON_TAB(, nTabBefore + 1));
    PrintCondition   (outstream, node->right    ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream                 ON_TAB(, nTabBefore + 1));

    PrintRightBracket(outstream                  ON_TAB(, nTabBefore   ));

    ON_TAB(PrintSlashN(outstream));

    return;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintConditionElseIf(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    if (node->type == NodeArgType::connect)
    {
        PrintConditionElseIf(outstream, node->left  ON_TAB(, nTabBefore));
        PrintConditionElseIf(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    PrintBefore      (outstream                   ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",            condition_else_if       );
    PrintAfter       (outstream                                           );
    PrintLeftBracket (outstream                   ON_TAB(, nTabBefore    ));
    
    PrintBefore      (outstream                   ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",            condition               );
    PrintAfter       (outstream                                           );
    PrintLeftBracket (outstream                   ON_TAB(, nTabBefore + 1));
    PrintAssign      (outstream, node->left       ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream                   ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                         ));

    PrintBefore      (outstream                   ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",            body                    );
    PrintAfter       (outstream                                           );
    PrintLeftBracket (outstream                   ON_TAB(, nTabBefore + 1));
    PrintCondition   (outstream, node->right      ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream                   ON_TAB(, nTabBefore + 1));

    PrintRightBracket(outstream                   ON_TAB(, nTabBefore    ));

    ON_TAB(PrintSlashN(outstream                                         ));

    // return PrintConditionElseIf(outstream, node->right ON_TAB(, nTabBefore));
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintConditionElse(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    if (node->type != NodeArgType::condition || node->data.condition != Condition::else_t)
        EXIT(EXIT_FAILURE, "here must be condition 'else'");

    PrintBefore      (outstream               ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",        condition_else          );
    PrintAfter       (outstream                                       );
    PrintLeftBracket (outstream               ON_TAB(, nTabBefore    ));
    
    PrintBefore      (outstream               ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",        body                    );
    PrintAfter       (outstream                                       );
    PrintLeftBracket (outstream               ON_TAB(, nTabBefore + 1));
    PrintCondition    (outstream, node->right ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream               ON_TAB(, nTabBefore + 1));
    
    PrintRightBracket(outstream               ON_TAB(, nTabBefore    ));
    
    ON_TAB(PrintSlashN(outstream                                     ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintCycle(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintCycle(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCycle(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type == NodeArgType::cycle && node->data.cycle == Cycle::while_t)
        return PrintCycleWhile(outstream, node ON_TAB(, nTabBefore));

    if (type == NodeArgType::cycle && node->data.cycle == Cycle::for_t)
        return PrintCycleFor(outstream, node ON_TAB(, nTabBefore));

    return PrintReturn(outstream, node ON_TAB(, nTabBefore));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintCycleWhile(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintCycleWhile(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCycleWhile(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::cycle || node->data.cycle != Cycle::while_t)
        EXIT(EXIT_FAILURE, "here must be cycle 'while'");

    PrintBefore      (outstream             ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",      cycle_while             );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore    ));
    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",      cycle_condition         );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    PrintAssign      (outstream, node->left ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));

    ON_TAB(PrintSlashN(outstream                                   ));

    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",      body                     );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    PrintCondition   (outstream, node->right ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));

    PrintRightBracket(outstream             ON_TAB(, nTabBefore    ));

    ON_TAB(PrintSlashN(outstream                                   ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintCycleFor(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintCycleFor(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCycleFor(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::cycle || node->data.cycle != Cycle::for_t)
        EXIT(EXIT_FAILURE, "here must be cycle 'for'");

    PrintBefore      (outstream             ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",      cycle_for               );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore    ));
    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",      cycle_condition         );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    PrintDefVariable (outstream, node->left ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));

    ON_TAB(PrintSlashN(outstream                                   ));

    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "%s",      body                    );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    PrintCondition   (outstream, node->right ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));

    PrintRightBracket(outstream             ON_TAB(, nTabBefore    ));

    ON_TAB(PrintSlashN(outstream                                   ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintReturn(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    ON_DEBUG(
    NODE_GRAPHIC_DUMP(node);
    )

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintReturn(outstream, node->left  ON_TAB(, nTabBefore));
        PrintReturn(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::attribute || node->data.attribute != FunctionAttribute::ret)
        return PrintDefVariable(outstream, node ON_TAB(, nTabBefore));

    PrintBefore      (outstream             ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",      ret                     );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore    ));
    PrintOperation   (outstream, node->left ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore    ));

    ON_TAB(PrintSlashN(outstream                                   ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintDefVariable(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintDefVariable(outstream, node->left  ON_TAB(, nTabBefore));
        PrintDefVariable(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::initialisation || node->data.init != Initialisation::def_variable)
        return PrintAssign(outstream, node ON_TAB(, nTabBefore));


    PrintBefore       (outstream                                ON_TAB(, nTabBefore    ));
    fprintf           (outstream, "%s",                         define_variable         );
    PrintAfter        (outstream                                                        );
    PrintLeftBracket  (outstream                                ON_TAB(, nTabBefore    ));
    PrintType         (outstream, node->left                    ON_TAB(, nTabBefore + 1));
    PrintName         (outstream, node->left->left              ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                                       ));

    PrintBefore       (outstream                                ON_TAB(, nTabBefore + 1));
    fprintf           (outstream, "%s",                         assign                   );
    PrintAfter        (outstream                                                        );
    PrintLeftBracket  (outstream                                ON_TAB(, nTabBefore + 1));
    PrintOperation    (outstream, node->right                   ON_TAB(, nTabBefore + 2));
    PrintRightBracket (outstream                                ON_TAB(, nTabBefore + 1));
    
    PrintRightBracket (outstream                                ON_TAB(, nTabBefore    ));
    ON_TAB(PrintSlashN(outstream                                                       ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintAssign(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintAssign(outstream, node->left  ON_TAB(, nTabBefore));
        PrintAssign(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::initialisation || node->data.init != Initialisation::assign_variable)
        return PrintOperation(outstream, node ON_TAB(, nTabBefore));


    PrintBefore      (outstream                         ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",                  assign                  );
    PrintAfter       (outstream                                                 );
    PrintLeftBracket (outstream                         ON_TAB(, nTabBefore    ));
    PrintName        (outstream, node->left             ON_TAB(, nTabBefore + 1));
    PrintOperation   (outstream, node->right            ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream                         ON_TAB(, nTabBefore    ));

    ON_TAB(PrintSlashN(outstream                                               ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintOperation(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintOperation(outstream, node->left  ON_TAB(, nTabBefore));
        PrintOperation(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::operation)
        return PrintCallFunction(outstream, node ON_TAB(, nTabBefore));

    PrintBefore(outstream ON_TAB(, nTabBefore));

    fprintf(outstream, "%s ", operation);

    Operation operation = node->data.oper;

    switch (operation)
    {
        case Operation::assign:            fprintf(outstream, "%s", assign_operation          ); break;
        case Operation::plus:              fprintf(outstream, "%s", plus_operation            ); break;
        case Operation::minus:             fprintf(outstream, "%s", minus_operation           ); break;
        case Operation::mul:               fprintf(outstream, "%s", mul_operation             ); break;
        case Operation::dive:              fprintf(outstream, "%s", div_operation             ); break;
        case Operation::power:             fprintf(outstream, "%s", power_operation           ); break;
        case Operation::equal:             fprintf(outstream, "%s", equal_operation           ); break;
        case Operation::not_equal:         fprintf(outstream, "%s", not_equal_operation       ); break;
        case Operation::greater:           fprintf(outstream, "%s", greater_operation         ); break;
        case Operation::greater_or_equal:  fprintf(outstream, "%s", greater_or_equal_operation); break;
        case Operation::less:              fprintf(outstream, "%s", less_operation            ); break;
        case Operation::less_or_equal:     fprintf(outstream, "%s", less_or_equal_operation   ); break;
        case Operation::bool_and:          fprintf(outstream, "%s", bool_and_operation        ); break;
        case Operation::bool_or:           fprintf(outstream, "%s", bool_or_operation         ); break;
        case Operation::bool_not:          fprintf(outstream, "%s", bool_not_operation        ); break;
        case Operation::plus_plus:         fprintf(outstream, "%s", plus_plus_operation       ); break; 
        case Operation::minus_minus:       fprintf(outstream, "%s", minus_minus_operation     ); break;
        case Operation::plus_equal:        fprintf(outstream, "%s", plus_equal_operation      ); break;
        case Operation::minus_equal:       fprintf(outstream, "%s", minus_minus_operation     ); break;
        case Operation::mul_equal:         fprintf(outstream, "%s", mul_equal_operation       ); break;
        case Operation::div_equal:         fprintf(outstream, "%s", div_equal_operation       ); break;
        case Operation::undefined_operation:
        default:  EXIT(EXIT_FAILURE, "undefined operation type.");
    }

    PrintAfter        (outstream                                     );

    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore    ));

    PrintOperation    (outstream, node->left  ON_TAB(, nTabBefore + 1));
    PrintOperation    (outstream, node->right ON_TAB(, nTabBefore + 1));

    PrintRightBracket (outstream              ON_TAB(, nTabBefore    ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintCallFunction(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintCallFunction(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCallFunction(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (node->data.init != Initialisation::call_function)
        return PrintNumber(outstream, node ON_TAB(, nTabBefore));

    PrintBefore           (outstream                       ON_TAB(, nTabBefore    ));
    fprintf               (outstream, "%s",                call_function           );
    PrintAfter            (outstream                                               );
    PrintLeftBracket      (outstream                       ON_TAB(, nTabBefore    ));

    PrintName             (outstream, node->left           ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN    (outstream                                              ));
    
    PrintCallFunctionArgs (outstream, node->left->left     ON_TAB(, nTabBefore + 1));
    PrintRightBracket     (outstream                       ON_TAB(, nTabBefore    )); 

    ON_TAB(PrintSlashN    (outstream                                              ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static void PrintCallFunctionArgs(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(outstream);

    if (!node) return;

    PrintBefore      (outstream                       ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "%s",                call_function_arguments );
    PrintAfter       (outstream                                               );

    PrintLeftBracket (outstream                       ON_TAB(, nTabBefore    ));
    PrintAssign      (outstream, node                 ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream                       ON_TAB(, nTabBefore    )); 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintNumber(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(node);
    assert(outstream);

    NodeArgType node_type = node->type;

    if (node_type != NodeArgType::number)
        return PrintName(outstream, node ON_TAB(, nTabBefore));

    PrintBefore(outstream ON_TAB(, nTabBefore));

    fprintf(outstream, "%s ", number);

    Type        num_type = node->data.num.type;
    NumberValue value    = node->data.num.value;

    // switch (num_type)
    // {
    //     case Type::int_type:    fprintf(outstream, "%d", *(int   *) value); break;
    //     case Type::char_type:   fprintf(outstream, "%c", *(char  *) value); break;
    //     case Type::double_type: fprintf(outstream, "%f", *(double*) value); break;
    //     case Type::void_type:
    //     case Type::undefined_type:
    //     default: EXIT(EXIT_FAILURE, "undef num type.");
    // }

    switch (num_type)
    {
        case Type::int_type:    fprintf(outstream, "%d", value.int_val);    break;
        case Type::char_type:   fprintf(outstream, "%c", value.char_val);   break;
        case Type::double_type: fprintf(outstream, "%f", value.double_val); break;
        case Type::void_type:
        case Type::undefined_type:
        default: EXIT(EXIT_FAILURE, "undef num type.");
    }

    PrintAfter(outstream);

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintName(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(node);
    assert(outstream);

    PrintBefore(outstream ON_TAB(, nTabBefore));

    if (node->type != NodeArgType::name)
        EXIT(EXIT_FAILURE, "here must be node with type 'name'");

    fprintf(outstream, "%s ", name);

    const char* node_name    = node->data.name.name.name;
    size_t      node_name_len = node->data.name.name.len;

    for (size_t i = 0; i < node_name_len; i++)
        fprintf(outstream, "%c", node_name[i]);

    PrintAfter(outstream);

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintType(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    WHERE_PRINT_TREE_IS();

    assert(node);
    assert(outstream);

    PrintBefore(outstream ON_TAB(, nTabBefore));

    if (node->type != NodeArgType::type)
        EXIT(EXIT_FAILURE, "here node arg type must be 'type'");

    fprintf(outstream, "%s ", type);

    Type node_type = node->data.type;
    switch (node_type)
    {
        case Type::int_type:       fprintf(outstream, "%s", int_type   ); break;
        case Type::char_type:      fprintf(outstream, "%s", char_type  ); break;
        case Type::double_type:    fprintf(outstream, "%s", double_type); break;
        case Type::void_type:      fprintf(outstream, "%s", void_type  ); break;
        case Type::undefined_type:
        default: EXIT(EXIT_FAILURE, "undef type of node arg type");
    }

    PrintAfter(outstream);

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintLeftBracket(FILE* outstream ON_TAB(, size_t nTab))
{
    assert(outstream);
    
    ON_TAB(
    PrintNTab(outstream, nTab);
    )

    fprintf(outstream, "{");

    PrintAfter(outstream);

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintRightBracket(FILE* outstream ON_TAB(, size_t nTab))
{
    assert(outstream);

    ON_TAB(
    PrintNTab(outstream, nTab);
    )

    fprintf(outstream, "}");

    PrintAfter(outstream);

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_TAB(

static void PrintNTab(FILE* outstream, size_t nTab)
{
    assert(outstream);
    for (size_t i = 0; i < nTab; i++)
    {
        fprintf(outstream, "\t");
    }

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintSlashN(FILE* outstream)
{
    assert(outstream);

    fprintf(outstream, "\n");

    return;
}

)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OFF_TAB(

static void PrintSpace(FILE* outstream)
{
    assert(outstream);

    fprintf(outstream, " ");

    return;
}

)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintBefore(FILE* outstream ON_TAB(, size_t nTab))
{
    assert(outstream);

    ON_TAB(
    PrintNTab(outstream, nTab);
    )

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintAfter(FILE* outstream)
{
    assert(outstream);

    ON_TAB(
    PrintSlashN(outstream);
    )

    OFF_TAB(
    PrintSpace(outstream);
    )

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_WRITE_TREE_DEBUG
(
void WherePrintTreeIs(const char* func)
{
    assert(func);

    ON_DEBUG(
    LOG_PRINT(Yellow, "write_tree::%s\n", func);
    )
    return;
}
)