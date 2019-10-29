#pragma once
#include <stdio.h>
#include "../../cliparser/include/cli.h"
int encodeSampleAdaptive(unsigned long sampleToEncode, unsigned int * counter, unsigned int * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
long extractBits(long number, int k);
void writeBits(unsigned long writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream);
