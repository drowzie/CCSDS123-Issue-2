#pragma once
#include <stdio.h>
#include "../../cliparser/include/cli.h"
int encodeSampleAdaptive(unsigned long sampleToEncode, unsigned int * counter, unsigned int * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
void writeBits(unsigned int writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream);