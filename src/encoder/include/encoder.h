#pragma once
#include <stdio.h>
#include "uthash.h"
#include "../../cliparser/include/cli.h"

typedef struct codeWord {
    char inputCodeword[256]; //Input codeword
    uint32_t codeWordValue; // Output Codeword
    uint8_t bitSize; //given bitsize
    uint8_t ifReverse; // If ouput should be reversed (0/1)
    UT_hash_handle hh;
} codeWord;

int encodeSampleAdaptive(unsigned long sampleToEncode, unsigned int * counter, unsigned int * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
long extractBits(long number, int k);
void writeBits(unsigned long writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream);
codeWord *searchHash(char searchString[256], int table);
void hashCodeTableValues();

