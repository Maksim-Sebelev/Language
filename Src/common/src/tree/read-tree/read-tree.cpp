#define WORD_ARRAY_POINTER         // for struct WordArray in read-file/read-file.hpp
#define WORD_SAVE_INPUT_STREAM     // for struct WordArray in read-file/read-file.hpp

#include <string.h>
#include <assert.h>
#include "tree/tree.hpp"
#include "tree/read-tree/read-tree.hpp"

#include "read-file/read-file.hpp"

#ifdef _DEBUG
#include "tree/tree-dump/tree-dump.hpp"
#include "log/log.hpp"
#endif // _DEBUG

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* const bad_signature_massage = "Bad signature. Possible reason - that incorrect tree text format for this compiler.";

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignature        (WordArray* wordArr);
static void CheckSignatureBegin   (WordArray* wordArr);
static void CheckSignatureName    (WordArray* wordArr);
static void CheckSignatureAutor   (WordArray* wordArr);
static void CheckSignatureVersion (WordArray* wordArr);

static void CheckSignatureHelper  (WordArray* wordArr, const char* correct);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tree_t ReadTree(WordArray* wordArr)
{
    assert(wordArr);

    CheckSignature(wordArr);


    Tree_t tree = {};

    

    return tree;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignature(WordArray* wordArr)
{
    assert(wordArr);

    CheckSignatureBegin   (wordArr);
    CheckSignatureName    (wordArr);
    CheckSignatureAutor   (wordArr);
    CheckSignatureVersion (wordArr);


    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignatureBegin(WordArray* wordArr)
{
    assert(wordArr);

    CheckSignatureHelper(wordArr, "file");
    CheckSignatureHelper(wordArr, "signature:");

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignatureName(WordArray* wordArr)
{
    assert(wordArr);

    CheckSignatureHelper(wordArr, "name:");
    CheckSignatureHelper(wordArr, "ast");
    CheckSignatureHelper(wordArr, "txt");
    CheckSignatureHelper(wordArr, "format");

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignatureAutor(WordArray* wordArr)
{
    assert(wordArr);

    CheckSignatureHelper(wordArr, "autor:");
    CheckSignatureHelper(wordArr, "Sebelev");
    CheckSignatureHelper(wordArr, "M.");
    CheckSignatureHelper(wordArr, "M.");

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignatureVersion(WordArray* wordArr)
{
    assert(wordArr);

    CheckSignatureHelper(wordArr, "version:");
    CheckSignatureHelper(wordArr, "1.0");

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void CheckSignatureHelper(WordArray* wordArr, const char* correct)
{
    assert(wordArr);
    assert(correct);

    Word word = wordArr->words[wordArr->pointer];
    if (strcmp(word.word, correct) != 0)
    {
        EXIT(EXIT_FAILURE,  "%s\n"
                            "'%s' - here must be '%s'\n"
                            "%s:%lu:%lu\n",
                            bad_signature_massage, 
                            word.word, correct,
                            wordArr->input_stream, word.line, word.inLine
            );
    }

    wordArr->pointer++;

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

