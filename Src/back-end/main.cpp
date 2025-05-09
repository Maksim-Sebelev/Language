#define WORD_SAVE_INPUT_STREAM

#include <stdlib.h>
#include "tree/tree.hpp"
#include "assembler/assembler.hpp"
#include "processor/processor.hpp"
#include "tree/read-tree/read-tree.hpp"
#include "read-file/read-file.hpp"

#ifdef _DEBUG
#include "tree/tree-dump/tree-dump.hpp"
#include "log/log.hpp"
#endif // _DEBUG

int main()
{
    
    ON_DEBUG(
    COLOR_PRINT(GREEN, "\n\nBACKEND START\n\n");
    LOG_OPEN();
    )

    const char* tree_ast = "govno";

    COLOR_PRINT(YELLOW, "tree.ast = '%s'\nptr = %p\n", tree_ast, tree_ast);

    WordArray wordArr = ReadBufferFromFile("govno");

    ON_DEBUG(
    
    LOG_PRINT(Red, "size = %lu\n", wordArr.size);

    for (size_t i = 0; i < wordArr.size; i++)
    {
        Word word = wordArr.words[i];
        LOG_PRINT(Yellow, "word[%1lu] = \n{", i);
        LOG_PRINT(Green, "\tint word = '%f'\n\tlen = %lu\n\t%s:%lu:%lu\n", WordToDouble(&word), word.len, ON_WORD_SAVE_INPUT_STREAM(wordArr.input_stream,) word.line, word.inLine);
        LOG_PRINT(Yellow, "}\n\n");
    }

    )


    Tree_t tree = ReadTree(&wordArr);

    ON_DEBUG(
    TREE_GRAPHIC_DUMP(&tree);
    )




    ON_DEBUG(
    COLOR_PRINT(GREEN, "\n\nBACKEND END\n\n");
    LOG_CLOSE();
    )

    return EXIT_SUCCESS;
}
