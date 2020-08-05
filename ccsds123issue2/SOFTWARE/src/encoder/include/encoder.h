#pragma once
#include <stdio.h>
#include "uthash.h"
#include "../../cliparser/include/cli.h"

typedef struct codeWord {
    char * inputCodeword; //Input codeword
    uint64_t codeWordValue; // Output Codeword
    int bitSize; //given bitsize
    UT_hash_handle hh;
} codeWord;

int encodeSampleAdaptive(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
int encodeHybrid(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
void encodeFinalStage(uint64_t * accumulator,  unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);

// Hash specifics
codeWord *searchHash(char * searchString, int table);
void hashCodeTableValues();
void hashFlushCodes();
codeWord *searchFlushHash(char * searchString, int table);


//Utils
void fillBits(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, struct arguments * parameters, FILE * stream);
void writeBits(uint64_t writeBits, uint64_t numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream);
uint64_t extractBits(uint64_t number, int k);
void writeImageHeader(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * compressedImage, struct arguments * parameters);
void readImageHeader(FILE * compressedImage, struct arguments * parameters);
uint64_t computeLog2 (uint64_t x);

// For decoder
uint32_t decodeSampleAdaptive(uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, FILE * compressedImage, struct arguments * parameters);
uint32_t readBits(unsigned int numBits, FILE * compressedImage);
uint64_t readNZeros(uint16_t maxBits, FILE * compressedImage);

