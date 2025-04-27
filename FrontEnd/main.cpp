#include <stdio.h>
#include <stdlib.h>
#include "tree/tree.hpp"
#include "log/log.hpp"
#include "lib/lib.hpp"
#include "tree/treeDump/treeDump.hpp"

int main()
{
    printf("Start\n\n");

    ON_DEBUG(
    OPEN_LOG();
    )

    const char* input = "programm/programm.smm";

    Tree_t tree = {};

    TREE_ASSERT(TreeCtor(&tree, input));

    TREE_GRAPHIC_DUMP(&tree);

    
    TREE_ASSERT(TreeDtor(&tree));

    ON_DEBUG(
    CLOSE_LOG();
    )

    printf("\nEnd\n");

    return EXIT_SUCCESS;
}
