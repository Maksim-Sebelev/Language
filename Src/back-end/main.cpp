#define WORD_SAVE_INPUT_STREAM // for 'read-file/read-file.hpp'

#include <stdlib.h>
#include "read-file/read-file.hpp"
#include "tree/tree.hpp"
#include "assembler/assembler.hpp"
#include "processor/processor.hpp"
#include "tree/read-write-tree/read-tree/read-tree.hpp"

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

    const char* tree_ast = "tree/tree.ast";

    WordArray wordArr = ReadBufferFromFile(tree_ast);

    ON_DEBUG(

    LOG_PRINT(Red, "size = %lu\n", wordArr.size);

    for (size_t i = 0; i < wordArr.size; i++)
    {
        Word word = wordArr.words[i];
        LOG_PRINT(Yellow, "word[%lu] = \n{", i);
        LOG_PRINT(Green, "\tint word = '%s'\n\tlen = %lu\n\t%s:%lu:%lu\n", word.word, word.len, ON_WORD_SAVE_INPUT_STREAM(wordArr.input_stream,) word.line, word.inLine);
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


#undef WORD_SAVE_INPUT_STREAM
