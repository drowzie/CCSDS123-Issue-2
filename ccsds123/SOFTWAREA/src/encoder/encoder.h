#pragma once
#include <stdio.h>
#include "cliparser/cli.h"

int encodeSampleAdaptive(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, uint8_t * compressedImage, struct arguments * parameters);
uint32_t decodeSampleAdaptive(uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, FILE * compressedImage, struct arguments * parameters);

void writeImageHeader(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage, struct arguments * parameters);
void readImageHeader(FILE * compressedImage, struct arguments * parameters);

//Utils
void fillBits(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage, struct arguments * parameters);
void writeBits(uint32_t writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage);
long extractBits(long number, int k);

uint16_t readBits(unsigned int numBits, FILE * compressedImage);
uint16_t readNZeros(uint16_t maxBits, FILE * compressedImage);
