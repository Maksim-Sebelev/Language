#ifndef TREE_GRAPHIC_DUMP_HPP
#define TREE_GRAPHIC_DUMP_HPP


#include "tree/tree.hpp"


void NodeDump     (const Node_t* node, const char* file, const int line, const char* func);
void TreeDump     (const Tree_t* tree, const char* file, const int line, const char* func);

#define TREE_GRAPHIC_DUMP(tree) TreeDump     (tree, __FILE__, __LINE__, __func__)
#define NODE_GRAPHIC_DUMP(node) NodeDump     (node, __FILE__, __LINE__, __func__)


#endif // TREE_GRAPHIC_DUMP_HPP

