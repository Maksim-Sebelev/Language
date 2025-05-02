#include <stdlib.h>
#include <assert.h>
#include "lib/lib.hpp"
#include "tree/tree.hpp"
#include "tree/writeTree/writeTree.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#include "log/log.hpp"
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define _TAB

#ifdef _TAB
    #define ON_TAB(...) __VA_ARGS__
    #define OFF_TAB(...)
#else
    #define ON_TAB(...)
    #define OFF_TAB(...)  __VA_ARGS__
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static void PrintConditon          (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintConditonIf        (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintConditonElseIf    (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintConditonElse      (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
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
static void PrintInit         (FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore));
static void PrintSpace        (FILE* outstream                                                );
static void PrintBefore       (FILE* outstream                     ON_TAB(, size_t nTab      ));
static void PrintAfter        (FILE* outstream                                                );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintTree(const Tree_t* tree, const char* outstream)
{
    assert(tree);
    assert(outstream);

    FILE* out = fopen(outstream, "wb");

    if (!out) EXIT(EXIT_FAILURE, "failed open '%s'", outstream);

    PrintOperation(out, tree->root ON_TAB(, 0));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintConditon(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintConditon(outstream, node->left  ON_TAB(, nTabBefore));
        PrintConditon(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    if (type != NodeArgType::condition)
        return PrintReturn(outstream, node ON_TAB(, nTabBefore));

    assert(node->data.condition == Condition::if_t);

    PrintConditonIf    (outstream, node ON_TAB(, nTabBefore));
    PrintConditonElseIf(outstream, node ON_TAB(, nTabBefore));
    PrintConditonElse  (outstream, node ON_TAB(, nTabBefore));

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintConditonIf(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    // if (type == NodeArgType::connect)
    // {
    //     PrintCycle(outstream, node->left  ON_TAB(, nTabBefore));
    //     PrintCycle(outstream, node->right ON_TAB(, nTabBefore));
    //     return;
    // }

    if (type == NodeArgType::cycle && node->data.cycle == Cycle::while_t)
        return PrintCycleWhile(outstream, node ON_TAB(, nTabBefore));

    if (type == NodeArgType::cycle && node->data.cycle == Cycle::for_t)
        return PrintCycleFor(outstream, node ON_TAB(, nTabBefore));

    return PrintReturn(outstream, node ON_TAB(, nTabBefore));
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintConditonElseIf(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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

static void PrintConditonElse(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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

static void PrintCycle(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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
    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintCycleFor(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCycleFor(outstream, node->right ON_TAB(, nTabBefore));
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
    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                             );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    // PrintCondition   (outstream, node->right ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore    ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintCycleFor(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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
    PrintBefore      (outstream             ON_TAB(, nTabBefore + 1));
    fprintf          (outstream, "BODY"                             );
    PrintAfter       (outstream                                     );
    PrintLeftBracket (outstream             ON_TAB(, nTabBefore + 1));
    // PrintCondition   (outstream, node->right ON_TAB(, nTabBefore + 2));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream             ON_TAB(, nTabBefore    ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintReturn(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    assert(outstream);

    if (!node) return;

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

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintDefVariable(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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


    PrintBefore      (outstream                                ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "DEF_VAR"                                             );
    PrintAfter       (outstream                                                        );
    PrintLeftBracket (outstream                                ON_TAB(, nTabBefore    ));
    PrintType        (outstream, node->left                    ON_TAB(, nTabBefore + 1));
    PrintName        (outstream, node->left->left              ON_TAB(, nTabBefore + 1));
    PrintOperation   (outstream, node->left->left->left->left  ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream                                ON_TAB(, nTabBefore    ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintAssign(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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
    PrintOperation   (outstream, node->left->left->left ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream                         ON_TAB(, nTabBefore    ));

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintOperation(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
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
        case Operation::mul:               fprintf(outstream, "+" ); break;
        case Operation::dive:              fprintf(outstream, "+" ); break;
        case Operation::power:             fprintf(outstream, "+" ); break;
        case Operation::equal:             fprintf(outstream, "=="); break;
        case Operation::not_equal:         fprintf(outstream, ">="); break;
        case Operation::greater:           fprintf(outstream, "> "); break;
        case Operation::greater_or_equal:  fprintf(outstream, ">+"); break;
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
    PrintCallFunctionArgs (outstream, node->left           ON_TAB(, nTabBefore + 1));
    PrintRightBracket     (outstream                       ON_TAB(, nTabBefore    )); 

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static void PrintCallFunctionArgs(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    assert(outstream);

    if (!node) return;

    NodeArgType type = node->type;

    if (type == NodeArgType::connect)
    {
        PrintCallFunctionArgs(outstream, node->left  ON_TAB(, nTabBefore));
        PrintCallFunctionArgs(outstream, node->right ON_TAB(, nTabBefore));
        return;
    }

    PrintBefore      (outstream                       ON_TAB(, nTabBefore    ));
    fprintf          (outstream, "CALL_FUNCTION_ARGS"                         );
    PrintAfter       (outstream                                               );
    PrintLeftBracket (outstream                       ON_TAB(, nTabBefore    ));
    PrintAssign      (outstream, node->left           ON_TAB(, nTabBefore + 1));
    PrintRightBracket(outstream                       ON_TAB(, nTabBefore    )); 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintNumber(FILE* outstream, const Node_t* node ON_TAB(, size_t nTabBefore))
{
    assert(node);
    assert(outstream);

    NodeArgType node_type = node->type;

    if (node_type != NodeArgType::number)
        return PrintName(outstream, node ON_TAB(, nTabBefore));

    PrintBefore(outstream ON_TAB(, nTabBefore));

    fprintf(outstream, "NUM: ");

    Type        num_type = node->data.num.type;
    NumberValue num      = node->data.num.value;

    switch (num_type)
    {
        case Type::int_type:    fprintf(outstream, "%d", num.int_val   ); break;
        case Type::char_type:   fprintf(outstream, "%c", num.char_val  ); break;
        case Type::double_type: fprintf(outstream, "%f", num.double_val); break;
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
    assert(node);
    assert(outstream);

    PrintBefore(outstream ON_TAB(, nTabBefore));

    NodeArgType node_type = node->type;

    if (node_type != NodeArgType::name)
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

static void PrintSpace(FILE* outstream)
{
    assert(outstream);

    fprintf(outstream, " ");

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
