#include "include/encoder.h"
#include "../cliparser/include/cli.h"
#include "../predictor/include/predictor.h"
#include <math.h>


void initSampleEncoder(uint16_t z, uint16_t * counter, uint64_t * accumulator, struct arguments * parameters) {
    counter[z] = 1 << parameters->initialY;
	accumulator[z] = ((counter[z] * ((3 * (1 << (parameters->initialK+6))) - 49)) >> 7);
}

int encodeSampleAdaptive(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    if(x == 0 && y == 0) {
        initSampleEncoder(z, counter, accumulator, parameters);
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
    } else {
        long kValue = log2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > (parameters->dynamicRange - 2) ? parameters->dynamicRange - 2 : kValue;
        long uValue = sampleToEncode >> kValue;
        if(uValue < parameters->uMax) {
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
            writeBits(extractBits(sampleToEncode, kValue), kValue, numWrittenBits, totalWrittenBytes, fileToWrite);
        } else {
            writeBits(0, parameters->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        }
        if(x+y != 0) {
            if(counter[z] < ((1 << parameters->yStar) - 1)) {
                accumulator[z] += sampleToEncode;
                counter[z]++;
            } else{
                accumulator[z] = (accumulator[z] + sampleToEncode + 1) >> 1;
                counter[z] = (counter[z] + 1) >> 1;
            }
        }
    }

    return 0;
}