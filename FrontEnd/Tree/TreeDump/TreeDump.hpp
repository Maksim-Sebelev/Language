#ifndef TREE_GRAPHIC_DUMP_H
#define TREE_GRAPHIC_DUMP_H


#include "../Tree.hpp"


void TreeDump     (const Tree_t* tree,                    const char* file, const int line, const char* func);
void NodeTextDump (const Node_t* node, NameTable_t table, const char* file, const int line, const char* func);


#define TREE_GRAPHIC_DUMP(node) TreeDump     (node, __FILE__, __LINE__, __func__)
#define TEXT_NODE_DUMP(   node) NodeTextDump (node, __FILE__, __LINE__, __func__)


#endif
