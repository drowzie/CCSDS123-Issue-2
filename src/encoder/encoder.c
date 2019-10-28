#include "include/encoder.h"
#include "../cliparser/include/cli.h"
#include "../predictor/utils/include/utilities.h"

#include <stdio.h>
#include <math.h>
int encodeSampleAdaptive(unsigned long sampleToEncode, unsigned int * counter, unsigned int * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    if(y == 0 && x == 0) {
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
    } else {

        unsigned long kValue = (int) log2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > parameters->dynamicRange-2 ? parameters->dynamicRange-2 : kValue;
        unsigned int remainder = sampleToEncode & (((unsigned short)0xFFFF) >> (16 - kValue));
        unsigned int uValue = sampleToEncode / (0x1 << kValue);
        
        if(uValue < parameters->uMax) {
            writeBits(0x0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(0x1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
            writeBits(remainder, kValue, numWrittenBits, totalWrittenBytes, fileToWrite);
        } else {
            writeBits(0x0, parameters->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        }

        if(counter[z] < ((0x1 << parameters->yStar) - 1)) {
            accumulator[z] += sampleToEncode;
            counter[z]++;
        } else{
            accumulator[z] = (accumulator[z] + sampleToEncode + 1) >> 1;
            counter[z] = (counter[z] + 1) >> 1;
        }
    }

    return 0;
}


/*
    write numBits of writeBits into the file stream. numWrittenBits and storedValue 
*/

unsigned char storedValue = 0;

void writeBits(unsigned long writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream){
    for(int i = numBits - 1; i >= 0; i--){
        storedValue |= ((writeBits >> i) & 0x1) << (7 - (*numWrittenBits));
        (*numWrittenBits)++;
        if(*numWrittenBits >= 8){
            fwrite(&storedValue, 1, 1, stream);
            *numWrittenBits = 0;
            (*totalWrittenBytes)++;
            storedValue = 0;
        }
    }
}