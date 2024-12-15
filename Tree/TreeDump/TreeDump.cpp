#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "TreeDump.hpp"
#include "../Tree.hpp"
#include "../ReadTree/Tokens/Token.hpp"
#include "../../Common/GlobalInclude.hpp"
#include "../../Common/Dump/GlobalDump.hpp"
#include "../NameTable/NameTable.hpp"


static void DotNodeBegin          (FILE* dotFile);
static void DotCreateAllNodes     (FILE* dotFile, const Node_t* node);
static void DotCreateEdges        (FILE* dotFile, const Node_t* node);
static void DotCreateEdgesHelper  (FILE* dotFile, const Node_t* node);
static void TreeDumpHelper        (const Node_t* node, const char* dotFileName, const char* file, const int line, const char* func);

static const char* GetNodeColor       (const Node_t* node);
static const char* GetNodeTypeInStr   (const Node_t* node);
static const char* GetNodeDataInStr   (const Node_t* node);

static const double eps = 1e-50;

//=============================== Tree Dump ==================================================================================================================================================

void NodeTextDump(const Node_t* node, const char* file, const int line, const char* func)
{
    assert(file);
    assert(func);

    COLOR_PRINT(GREEN, "\nNode Dump:\n\n");

    PRINT_PLACE(YELLOW, file, line, func);
    
    COLOR_PRINT(GREEN, "\nnode:\n");

    if (!node)
    {
        COLOR_PRINT(RED, "'nullptr'\n\n");
        return;
    }

    COLOR_PRINT(CYAN,  "type = '%s'\n", GetNodeTypeInStr  (node));

    NodeArgType type = node->type;


    COLOR_PRINT(CYAN, "data = '");

    if (type == NodeArgType::number)
    {
        COLOR_PRINT(CYAN, "%lf", node->data.num);
    }

    else if (type == NodeArgType::name)
    {
        const char* name     = node->data.name.name;
        size_t nameLen = node->data.name.nameLen;

        for (size_t i = 0; i < nameLen; i++)
        {
            COLOR_PRINT(CYAN, "%c", name[i]);
        }
    }

    else
    {
        COLOR_PRINT(CYAN,  "%s", GetNodeDataInStr(node));
    }

    COLOR_PRINT(CYAN, "'\n");

    COLOR_PRINT(VIOLET, "left  = %p\n", node->left);
    COLOR_PRINT(VIOLET, "right = %p\n", node->right);

    COLOR_PRINT(GREEN, "\nNode Dump End.\n\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TreeDump(const Node_t* node, const char* file, const int line, const char* func)
{
    assert(node);
    assert(file);
    assert(func);

    static size_t ImgQuant = 1;

    static const size_t MaxfileNameLen = 128;
    char outfile[MaxfileNameLen] = {};
    sprintf(outfile, "tree%lu.png", ImgQuant);
    ImgQuant++;

    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    static const char* dotFileName = "tree.dot";
    sprintf(command, "dot -Tpng %s > %s", dotFileName, outfile);

    TreeDumpHelper(node, dotFileName, file, line, func);
    system(command);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void TreeDumpHelper(const Node_t* node, const char* dotFileName, const char* file, const int line, const char* func)
{
    assert(node);
    assert(dotFileName);
    assert(file);
    assert(func);
    assert(true || line);

    FILE* dotFile = fopen(dotFileName, "w");
    assert(dotFile);

    DotNodeBegin(dotFile);

    DotCreateDumpPlace(dotFile, file, line, func);

    DotCreateAllNodes(dotFile, node);
    DotCreateEdges(dotFile, node);

    DotEnd(dotFile);

    fclose(dotFile);
    dotFile = nullptr;

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotNodeBegin(FILE* dotFile)
{
    assert(dotFile);
    fprintf(dotFile, "digraph G{\nrankdir=TB\ngraph [bgcolor=\"#000000\"];\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateAllNodes(FILE* dotFile, const Node_t* node)
{
    assert(dotFile);
    assert(node);


    const char* nodeColor = GetNodeColor(node);
    fprintf(dotFile, "node%p", node);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"%s\"", nodeColor);

    NodeArgType type = node->type;

    fprintf(dotFile, "label = \"");

    if (type == NodeArgType::number)
    {
        
        Number number = node->data.num;

        if (IsDoubleEqual(number, floor(number), eps))
        {
            fprintf(dotFile, "%d", (int) number);
        }
        else
        {
            fprintf(dotFile, "%lf", number);
        }
        
    }

    else if (type == NodeArgType::name)
    {
        const char* name = node->data.name.name;
        size_t nameLen   = node->data.name.nameLen;

        for (size_t i = 0; i < nameLen; i++)
        {
            fprintf(dotFile, "%c", name[i]);
        }
    }

    else
    {
        const char* arg = GetNodeDataInStr(node);
        fprintf(dotFile, "%s", arg);
    }

    fprintf(dotFile, "\", ");

    fprintf(dotFile, "color = \"#777777\"];\n");

    if (node->left)
    {
        DotCreateAllNodes(dotFile, node->left);
    }

    if (node->right)
    {
        DotCreateAllNodes(dotFile, node->right);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateEdges(FILE* dotFile, const Node_t* node)
{
    assert(dotFile);
    assert(node);

    fprintf(dotFile, "edge[color=\"#373737\"];\n");
    DotCreateEdgesHelper(dotFile, node);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateEdgesHelper(FILE* dotFile, const Node_t* node)
{
    assert(dotFile);
    assert(node);

    if (node->left)
    {
        fprintf(dotFile, "node%p->node%p;\n", node, node->left);

        DotCreateEdgesHelper(dotFile, node->left);
    }

    if (node->right)
    {
        fprintf(dotFile, "node%p->node%p;\n", node, node->right);

        DotCreateEdgesHelper(dotFile, node->right);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static const char* GetNodeColor(const Node_t* node)
{
    assert(node);

    NodeArgType type = node->type;

    switch (type)
    {
        case NodeArgType::number:    return "#1662b7";
        case NodeArgType::operation: return "#177d20";
        case NodeArgType::name:      return "#832316";
        case NodeArgType::function:  return "#218617";
        case NodeArgType::undefined: return "red";
        default:
            assert(0 && "undefined situation in GetColorType.\n");
            break;
    }

    assert(0 && "undefined situation in GetColorType.\n");

    return "red";    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetNodeTypeInStr(const Node_t* node)
{
    assert(node);

    NodeArgType type = node->type;

    switch (type)
    {
        case NodeArgType::number:
            return "number";

        case NodeArgType::operation:
            return "operation";

        case NodeArgType::function:
            return "function";

        case NodeArgType::name:
            return "name";

        case NodeArgType::undefined:
            return "undefined";

        default:
            assert(0 && "You forgot about some node type in text dump.\n");
            return "undefined";
    }
    assert(0 && "We must not be here.\n");
    return "wtf?";
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetNodeDataInStr(const Node_t* node)
{
    assert(node);

    NodeArgType type = node->type;

    switch (type)
    {
        case NodeArgType::number:
        {        
            assert(0 && "Number is drugaya situation.\n");
            return "undefined";
        }

        case NodeArgType::name:
        {
            assert(0 && "name is drugaja situation.");
            return "undefined";
        }

        case NodeArgType::operation:
        {
            Operation oper = node->data.oper;
            return GetOperationInStr(oper);
        }

        case NodeArgType::function:
        {
            Function func = node->data.func;
            return GetFuncInStr(func);
        }

        case NodeArgType::undefined:
            return "undefined node type.";

        default:
            assert(0 && "you forgot about some node type.");
            return "undefined";
    }

    assert(0 && "you forgot about some node type.\n");
    return "undefined";
}

//=============================== Tree Dump End =============================================================================================================================================

