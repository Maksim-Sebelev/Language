#include <stdlib.h>
#include <assert.h>
#include "lib/lib.hpp"
#include "tree/tree.hpp"
#include "tree/write-tree/write-tree.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#include "log/log.hpp"
#include "tree/tree-dump/tree-dump.hpp"
#endif // _DEBUG

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// #define _TAB

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

    if (!out) EXIT(EXIT_FAILURE, "failed open '%s'", outstream);

    PrintSignature(out);
    PrintDefFunc(out, tree->root ON_TAB(, 0));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintSignature(FILE* outstream)
{
    assert(outstream);

    fprintf(outstream,  "file signature:\n"
                        "name: ast txt format\n"
                        "autor: Sebelev M. M.\n"
                         "version: 1.0\n"
                         "\n"
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


    const Node_t* type = node->left;
    const Node_t* name = node->left->left;
    const Node_t* args = node->left->left->left;
    const Node_t* body = node->left->left->right;

    PrintBefore       (outstream              ON_TAB(, nTabBefore    ));
    fprintf           (outstream, "DEF_FUNC"                          );
    PrintAfter        (outstream                                      );
    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore    ));

    PrintType         (outstream, type        ON_TAB(, nTabBefore + 1));
    PrintName         (outstream, name        ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                     ));

    PrintBefore       (outstream              ON_TAB(, nTabBefore + 1));
    fprintf           (outstream, "ARGS"                              );
    PrintAfter        (outstream                                      );
    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore + 1));
    PrintDefFuncArg   (outstream, args        ON_TAB(, nTabBefore + 2));
    PrintRightBracket (outstream              ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                     ));
    PrintBefore       (outstream              ON_TAB(, nTabBefore + 1));
    fprintf           (outstream, "BODY"                              );
    PrintAfter        (outstream                                      );
    PrintLeftBracket  (outstream              ON_TAB(, nTabBefore + 1));
    PrintCondition     (outstream, body        ON_TAB(, nTabBefore + 2));
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
    fprintf          (outstream, "CONDITION: if"                        );
    PrintAfter       (outstream                                         );
    PrintLeftBracket (outstream                 ON_TAB(, nTabBefore    ));
    
    PrintBefore      (outstream                 ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "CONDITION"                            );
    PrintAfter       (outstream                                         );
    PrintLeftBracket (outstream                 ON_TAB(, nTabBefore + 1));
    PrintAssign      (outstream, node->left     ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream                 ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                       ));

    PrintBefore      (outstream                 ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                                 );
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
    fprintf          (outstream, "CONDITION: else_if"                     );
    PrintAfter       (outstream                                           );
    PrintLeftBracket (outstream                   ON_TAB(, nTabBefore    ));
    
    PrintBefore      (outstream                   ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "CONDITION"                              );
    PrintAfter       (outstream                                           );
    PrintLeftBracket (outstream                   ON_TAB(, nTabBefore + 1));
    PrintAssign      (outstream, node->left       ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream                   ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                         ));

    PrintBefore      (outstream                   ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                                   );
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
    fprintf          (outstream, "CONDITION: else"                    );
    PrintAfter       (outstream                                       );
    PrintLeftBracket (outstream               ON_TAB(, nTabBefore    ));
    
    PrintBefore      (outstream               ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                               );
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
    fprintf          (outstream, "CYCLE: WHILE"                     );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore    ));
    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "CYCLE_CONDITION"                  );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    PrintAssign      (outstream, node->left ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));

    ON_TAB(PrintSlashN(outstream                                   ));

    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                             );
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
    fprintf          (outstream, "CYCLE: FOR"                       );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore    ));
    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "CYCLE_CONDITION"                  );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    PrintDefVariable (outstream, node->left ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));

    ON_TAB(PrintSlashN(outstream                                   ));

    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                             );
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
    fprintf          (outstream,            "RETURN"                );
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
    fprintf           (outstream, "DEF_VAR"                                             );
    PrintAfter        (outstream                                                        );
    PrintLeftBracket  (outstream                                ON_TAB(, nTabBefore    ));
    PrintType         (outstream, node->left                    ON_TAB(, nTabBefore + 1));
    PrintName         (outstream, node->left->left              ON_TAB(, nTabBefore + 1));
    ON_TAB(PrintSlashN(outstream                                                       ));

    PrintBefore(outstream ON_TAB(, nTabBefore + 1));
    fprintf(outstream, "ASSIGN");
    PrintAfter(outstream);
    PrintLeftBracket(outstream ON_TAB(, nTabBefore + 1));
    PrintOperation    (outstream, node->right  ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream ON_TAB(, nTabBefore + 1));
    
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
    fprintf          (outstream, "ASGN"                                         );
    PrintAfter       (outstream                                                 );
    PrintLeftBracket (outstream                         ON_TAB(, nTabBefore    ));
    PrintName        (outstream, node->left             ON_TAB(, nTabBefore + 1));
    PrintOperation   (outstream, node->right            ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream                         ON_TAB(, nTabBefore    ));

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

    fprintf(outstream, "OP: ");

    Operation operation = node->data.oper;

    switch (operation)
    {
        case Operation::assign:            fprintf(outstream, "=" ); break;
        case Operation::plus:              fprintf(outstream, "+" ); break;
        case Operation::minus:             fprintf(outstream, "-" ); break;
        case Operation::mul:               fprintf(outstream, "*" ); break;
        case Operation::dive:              fprintf(outstream, "/" ); break;
        case Operation::power:             fprintf(outstream, "^" ); break;
        case Operation::equal:             fprintf(outstream, "=="); break;
        case Operation::not_equal:         fprintf(outstream, ">="); break;
        case Operation::greater:           fprintf(outstream, ">" ); break;
        case Operation::greater_or_equal:  fprintf(outstream, ">="); break;
        case Operation::less:              fprintf(outstream, "<" ); break;
        case Operation::less_or_equal:     fprintf(outstream, "<="); break;
        case Operation::bool_and:          fprintf(outstream, "&&"); break;
        case Operation::bool_or:           fprintf(outstream, "||"); break;
        case Operation::bool_not:          fprintf(outstream, "!" ); break;
        case Operation::plus_plus:         fprintf(outstream, "++"); break; 
        case Operation::minus_minus:       fprintf(outstream, "--"); break;
        case Operation::plus_equal:        fprintf(outstream, "+="); break;
        case Operation::minus_equal:       fprintf(outstream, "-="); break;
        case Operation::mul_equal:         fprintf(outstream, "*="); break;
        case Operation::div_equal:         fprintf(outstream, "/="); break;
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
    fprintf               (outstream, "CALL_FUNCTION"                              );
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
    fprintf          (outstream, "CALL_FUNCTION_ARGS"                         );
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

    fprintf(outstream, "NUM: ");

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

    fprintf(outstream, "NAME: ");

    const char* name    = node->data.name.name.name;
    size_t      nameLen = node->data.name.name.len;

    for (size_t i = 0; i < nameLen; i++)
        fprintf(outstream, "%c", name[i]);

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

    fprintf(outstream, "TYPE: ");

    Type type = node->data.type;
    switch (type)
    {
        case Type::int_type:       fprintf(outstream, "int"   ); break;
        case Type::char_type:      fprintf(outstream, "char"  ); break;
        case Type::double_type:    fprintf(outstream, "double"); break;
        case Type::void_type:      fprintf(outstream, "void"  ); break;
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