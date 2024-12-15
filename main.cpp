#include <stdio.h>
#include "Tree/Tree.hpp"
#include "Tree/TreeDump/TreeDump.hpp"

#include <malloc.h>

int main()
{
    const char* inputFile = "programm.txt";

    Tree_t tree = {};

    char* s = nullptr;

    TREE_ASSERT(TreeCtor(&tree, inputFile, &s));

    TREE_GRAPHIC_DUMP(tree.root);


    TREE_ASSERT(TreeDtor(&tree));


    // printf("\n\n");
    // for (size_t i = 0; i < 10; i++)
    // {
    //     printf("%c", *(s + i));
    // }
    // printf("\n\n");

    free(s);
    return EXIT_SUCCESS;
}