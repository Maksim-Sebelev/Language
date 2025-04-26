#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <malloc.h>
#include "lib/lib.hpp"
#include "lib/colorPrint.hpp"
#include "tree/readTree/readTreeGlobalnclude.hpp"
#include "tree/readTree/fileread/fileread.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t CalcFileSize(const char* file);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

InputData ReadFile(const char* inputFile, size_t* inputLen)
{
    assert(inputFile);

    FILE* inputStream = fopen(inputFile, "rb");
    if (!inputStream)
    {
        COLOR_PRINT(RED, "failed open: ");
        COLOR_PRINT(WHITE, "'%s'\n", inputFile);
        COLOR_PRINT(CYAN, "\nexit in 3, 2, 1...");
        exit(EXIT_FAILURE);
    }

    size_t fileSize = CalcFileSize(inputFile);

    char* buffer = (char*) calloc(fileSize + 1, sizeof(char));
    assert(buffer);

    size_t freadReturn = fread(buffer, sizeof(char), fileSize, inputStream);
    assert(freadReturn == fileSize);

    fclose(inputStream);

    buffer[fileSize] = '\0';

    *inputLen = fileSize;

    InputData inputData = {};

    inputData.inputStream = inputFile;
    inputData.buffer    = buffer;

    return inputData;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void InputDataDtor(InputData* inputData)
{
    assert(inputData);
    assert(inputData->buffer);
    assert(inputData->inputStream);

    FREE(inputData->buffer);
    inputData->inputStream = nullptr;

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t CalcFileSize(const char* file)
{
    assert(file);

    struct stat buf = {};
    stat(file, &buf);
    return (size_t) buf.st_size;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------