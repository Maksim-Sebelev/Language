#include <stdio.h>
#include <stdlib.h>
#include "tree/tree.hpp"
#include "lib/lib.hpp"

#ifdef _DEBUG
#include "log/log.hpp"
#include "tree/treeDump/treeDump.hpp"
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

    TREE_ASSERT(TreeDtor(&tree));

    ON_DEBUG(
    CLOSE_LOG();
    )

    ON_DEBUG(
    COLOR_PRINT(GREEN, "\nFRONTEND END\n");
    )

    return EXIT_SUCCESS;
}
