#include <stdio.h>
#include "Tree/Tree.hpp"
#include "Tree/TreeDump/TreeDump.hpp"


int main()
{
    const char* inputFile = "programm.txt";

    Tree_t tree = {};
    TREE_ASSERT(TreeCtor(&tree, inputFile));

    TREE_GRAPHIC_DUMP(tree.root);


    TREE_ASSERT(TreeDtor(&tree));
    
    return EXIT_SUCCESS;
}