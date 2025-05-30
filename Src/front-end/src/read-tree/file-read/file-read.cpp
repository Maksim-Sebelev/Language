#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <malloc.h>
#include "lib/lib.hpp"
#include "read-tree/file-read/file-read.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#include "log/log.hpp"
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t CalcFileSize(const char* file);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

InputData ReadFile(const char* inputFile)
{
    assert(inputFile);

    FILE* inputStream = fopen(inputFile, "rb");

    if (!inputStream)
        EXIT(EXIT_FAILURE, "failed open: '%s'\n", inputFile);

    size_t fileSize = CalcFileSize(inputFile);

    char* buffer = (char*) calloc(fileSize + 1, sizeof(char));
    
    if (!buffer)
        EXIT(EXIT_FAILURE, "failer calloc memory for buffer for input file");

    size_t freadReturn = fread(buffer, sizeof(char), fileSize, inputStream);
    if (freadReturn != fileSize)
        EXIT(EXIT_FAILURE, "falied read buffer with input programm");

    fclose(inputStream);

    buffer[fileSize] = '\0';

    
    InputData inputData = {};
    
    inputData.inputStream = inputFile;
    inputData.buffer      = buffer;
    inputData.size        = fileSize;

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