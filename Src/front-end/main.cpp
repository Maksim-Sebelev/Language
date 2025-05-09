#include <stdio.h>
#include <stdlib.h>
#include "tree/tree.hpp"
#include "lib/lib.hpp"
#include "read-tree/file-read/file-read.hpp"
#include "read-tree/tokens/tokens.hpp"
#include "read-tree/recursive-descent/recursive-descent.hpp"
#include "tree/write-tree/write-tree.hpp"

#define _DEBUG

#ifdef _DEBUG
#include "log/log.hpp"
#include "tree/tree-dump/tree-dump.hpp"
#include "read-tree/tokens/tokens-dump/tokens-dump.hpp"
#endif

int main()
{
    ON_DEBUG(
    COLOR_PRINT(GREEN, "FRONTEND START\n\n");
    LOG_OPEN();
    )


    const char* input  = "programm/programm.asm";
    const char* output = "tree/tree.ast";

    InputData buffer    = ReadFile(input);

    TokensArr tokensArr = ReadInputBuffer(&buffer);

    ON_DEBUG(TOKEN_GRAPHIC_DUMP(&tokensArr));

    Tree_t    tree      = {};
    tree.root           = GetTree(&tokensArr, &buffer);

    ON_DEBUG(TREE_GRAPHIC_DUMP(&tree));

    PrintTree(&tree, output);

    InputDataDtor(&buffer);
    TokenDtor    (&tokensArr);
    TreeDtor     (&tree);
    

    ON_DEBUG(
    LOG_CLOSE();
    COLOR_PRINT(GREEN, "\nFRONTEND END\n");
    )

    return EXIT_SUCCESS;
}
