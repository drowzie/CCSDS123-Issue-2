#include "include/encoder.h"
#include "../cliparser/include/cli.h"
#include "../predictor/utils/include/utilities.h"

#include <stdio.h>
#include <math.h>
int encodeSampleAdaptive(unsigned long sampleToEncode, unsigned int * counter, unsigned int * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    if(y == 0 && x == 0) {
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
    } else {
        int kValue = 0;
        int uValue = 0;

        kValue = (int) log2(accumulator[z] + (49*counter[z] >> 7)) / counter[z];
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > parameters->dynamicRange-2 ? parameters->dynamicRange-2 : kValue;
        uValue = sampleToEncode >> kValue;
        uValue = uValue < 0 ? 0 : uValue;
        if(uValue < parameters->uMax) {
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
            writeBits(sampleToEncode, kValue, numWrittenBits, totalWrittenBytes, fileToWrite);
        } else {
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        }

        if(counter[z] < (1 << parameters->initialY)) {
            accumulator[z] += sampleToEncode;
            counter[z]++;
        } else {
            accumulator[z] = (accumulator[z] + sampleToEncode + 1) << 1;
            counter[z] = (counter[z] + 1) << 1;
        }
    }

    return 0;
}


/*
    write numBits of writeBits into the file stream. numWrittenBits and storedValue 
*/

unsigned int storedValue = 0;

void writeBits(unsigned int writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream){
    for(int i = numBits - 1; i >= 0; i--){
        storedValue |= ((writeBits >> i) & 0x1) << (7 - *numWrittenBits);
        (*numWrittenBits)++;
        if(*numWrittenBits >= 8){
            fwrite(&storedValue, 1, sizeof(unsigned char), stream);
            *numWrittenBits = 0;
            *totalWrittenBytes++;
            storedValue = 0;
        }
    }
}