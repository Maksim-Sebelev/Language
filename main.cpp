#include <stdio.h>
#include "Tree/Tree.hpp"
#include "Tree/TreeDump/TreeDump.hpp"

#include <malloc.h>

int main()
{
    const char* inputFile = "programm.txt";

    Tree_t tree = {};
    TREE_ASSERT(TreeCtor(&tree, inputFile));

    TREE_GRAPHIC_DUMP(tree.root);


    TREE_ASSERT(TreeDtor(&tree));

    char* danger = nullptr;

    for (size_t i = 0; i < 10; i++)
    {
        printf("%c", *(danger + 0x504000000010 + i));
    }

    // free(danger + 0x504000000010);

    return EXIT_SUCCESS;
}