#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include "tree/treeDump/treeDump.hpp"
#include "tree/tree.hpp"
#include "lib/lib.hpp"
#include "tree/readTree/tokens/token.hpp"
#include "tree/treeDump/globalDump.hpp"
#include "tree/nameTable/nametable.hpp"


static void DotNodeBegin          (FILE* dotFile);
static void DotCreateAllNodes     (FILE* dotFile, const Node_t* node);
static void DotCreateEdges        (FILE* dotFile, const Node_t* node);
static void DotCreateEdgesHelper  (FILE* dotFile, const Node_t* node);
static void TreeDumpHelper        (const Tree_t* tree, const char* dotFileName, const char* file, const int line, const char* func);
static void NodeDumpHelper        (const Node_t* node, const char* dotFileName, const char* file, const int line, const char* func);


static const char* GetNodeColor       (const Node_t* node);
static const char* GetNodeTypeInStr   (const Node_t* node);
static const char* GetNodeDataInStr   (const Node_t* node);

static void PrintName(Name name);

static const double eps = 1e-50;

//=============================== Node Text Dump ==================================================================================================================================================

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
        COLOR_PRINT(CYAN, "%d", node->data.num.value.int_val);
    }

    else if (type == NodeArgType::name)
    {
        // NamePointer namePointer = node->data.name;
        // PrintName(namePointer, table);
        PrintName(node->data.name);
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

static void PrintName(Name name)
{
    const char* nameStr = name.name.name;
    size_t      nameLen = name.name.len;

    for (size_t i = 0; i < nameLen; i++)
    {
        COLOR_PRINT(CYAN, "%c", nameStr[i]);
    }

    return;
}

//=============================== Node Text Dump End ==================================================================================================================================================

//=============================== Tree Graphic Dump ==================================================================================================================================================

void NodeDump(const Node_t* node, const char* file, const int line, const char* func)
{
    assert(node);
    assert(file);
    assert(func);

    static size_t ImgQuant = 1;

    system("rm -rf dot/node/img/*");
    system("rm -rf dot/node/dot/*");

    system("mkdir -p dot/");
    system("mkdir -p dot/node/");
    system("mkdir -p dot/node/img/");
    system("mkdir -p dot/node/dot/");

    static const size_t MaxfileNameLen = 128;
    char outfile[MaxfileNameLen] = {};
    sprintf(outfile, "dot/node/img/node%lu.png", ImgQuant);
    
    static const size_t MaxCommandLen = 256;
    char dotFileName[MaxfileNameLen] = {};
    sprintf(dotFileName, "dot/node/dot/node%lu.dot", ImgQuant);

    NodeDumpHelper(node, dotFileName, file, line, func);
    
    char command[MaxCommandLen] = {};
    sprintf(command, "dot -Tpng %s > %s", dotFileName, outfile);
    system(command);

    ImgQuant++;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void NodeDumpHelper(const Node_t* node, const char* dotFileName, const char* file, const int line, const char* func)
{
    assert(node);
    assert(dotFileName);
    assert(file);
    assert(func);
    assert(line);

    FILE* dotFile = fopen(dotFileName, "w");
    assert(dotFile);

    DotNodeBegin(dotFile);

    DotCreateDumpPlace(dotFile, file, line, func);

    DotCreateAllNodes(dotFile, node);
    DotCreateEdges   (dotFile, node);

    DotEnd(dotFile);

    fclose(dotFile);
    dotFile = nullptr;

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TreeDump(const Tree_t* tree, const char* file, const int line, const char* func)
{
    assert(tree);
    assert(file);
    assert(func);

    system("rm -rf dot/tree/img/*");
    system("rm -rf dot/tree/dot/*");

    system("mkdir -p dot/");
    system("mkdir -p dot/tree/");
    system("mkdir -p dot/tree/img/");
    system("mkdir -p dot/tree/dot/");

    static size_t ImgQuant = 1;

    static const size_t MaxfileNameLen = 128;
    char outfile[MaxfileNameLen] = {};
    sprintf(outfile, "dot/tree/img/tree%lu.png", ImgQuant);
    
    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    char dotFileName[MaxfileNameLen] = {};
    sprintf(dotFileName, "dot/tree/dot/tree%lu.dot", ImgQuant);
    sprintf(command, "dot -Tpng %s > %s", dotFileName, outfile);
    
    TreeDumpHelper(tree, dotFileName, file, line, func);
    system(command);
    
    ImgQuant++;
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void TreeDumpHelper(const Tree_t* tree, const char* dotFileName, const char* file, const int line, const char* func)
{
    assert(tree);
    assert(dotFileName);
    assert(file);
    assert(func);
    assert(line);

    FILE* dotFile = fopen(dotFileName, "w");
    assert(dotFile);

    DotNodeBegin(dotFile);

    DotCreateDumpPlace(dotFile, file, line, func);

    DotCreateAllNodes(dotFile, tree->root);
    DotCreateEdges(dotFile, tree->root);

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
        FprintNumber(dotFile, number);
    }

    else if (type == NodeArgType::name)
    {
        Name name = node->data.name;
        FprintName(dotFile, name);
    }

    else
    {
        const char* arg = GetNodeDataInStr(node);
        if (!isalpha(arg[0])) fprintf(dotFile, "\\");
        
        fprintf(dotFile, "%s", arg);
    }

    fprintf(dotFile, "\", ");

    fprintf(dotFile, "color = \"#777777\"];\n");

    if (node->left)  DotCreateAllNodes(dotFile, node->left);
    if (node->right) DotCreateAllNodes(dotFile, node->right);

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
        case NodeArgType::connect:   return "#FFFACD";
        case NodeArgType::type:      return "#CD5C5C";
        case NodeArgType::condition:
        case NodeArgType::cycle:
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

        case NodeArgType::name:
            return "name";

        case NodeArgType::undefined:
            return "undefined";

        case NodeArgType::condition:
            return "condition";

        case NodeArgType::cycle:
            return "cycle";

        case NodeArgType::connect:
            return "connect";
        
        case NodeArgType::type:
            return "type";

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

        case NodeArgType::type:
        {
            Type typee = node->data.type;
            return GetTypeInStr(typee);
        }

        case NodeArgType::undefined:
            return "undefined node type.";

        case NodeArgType::connect:
            return "connect";

        case NodeArgType::condition:
            return "condition";

        default:
            assert(0 && "you forgot about some node type.");
            return "undefined";
    }

    assert(0 && "you forgot about some node type.\n");
    return "undefined";
}

//=============================== Tree Graphic Dump End ==================================================================================================================================================

