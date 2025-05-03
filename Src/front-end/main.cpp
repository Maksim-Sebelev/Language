#include <stdio.h>
#include <stdlib.h>
#include "tree/tree.hpp"
#include "lib/lib.hpp"
#include "tree/write-tree/write-tree.hpp"

#ifdef _DEBUG
#include "log/log.hpp"
#include "tree/tree-dump/tree-dump.hpp"
#endif

int main()
{
    ON_DEBUG(
    COLOR_PRINT(GREEN, "FRONTEND START\n\n");
    )

    ON_DEBUG(
    OPEN_LOG();
    )

    const char* input = "../../programm/programm.cpp";
    Tree_t tree = {};

    TREE_ASSERT(TreeCtor(&tree, input));

    ON_DEBUG(
    TREE_GRAPHIC_DUMP(&tree);
    )

    const char* output = "../tree/tree.ast";
    PrintTree(&tree, output);

    TREE_ASSERT(TreeDtor(&tree));

    ON_DEBUG(
    CLOSE_LOG();
    )

    ON_DEBUG(
    COLOR_PRINT(GREEN, "\nFRONTEND END\n");
    )

    return EXIT_SUCCESS;
}
