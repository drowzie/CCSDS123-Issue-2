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
int encodeHybrid(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
void encodeFinalStage(uint64_t * accumulator, int z,  unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);

// Hash specifics
codeWord *searchHash(char searchString[256], int table);
void hashCodeTableValues();
void hashFlushCodes();
codeWord *searchFlushHash(char searchString[256], int table);


//Utils
void fillBits(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, struct arguments * parameters, FILE * stream);
void writeBits(uint64_t writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream);
long extractBits(long number, int k);