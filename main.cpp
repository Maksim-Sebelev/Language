#include <stdio.h>
#include "Tree/Tree.h"
#include "Tree/TreeDump/TreeDump.h"
#include "Tree/ReadTree.h"



int main()
{
    const char* inputFile = "programm.txt";

    Tree_t tree = {};
    TREE_ASSERT(TreeCtor(&tree, inputFile));

    TREE_GRAPHIC_DUMP(tree.root);


    TREE_ASSERT(TreeDtor(&tree));
    
    return EXIT_SUCCESS;
}