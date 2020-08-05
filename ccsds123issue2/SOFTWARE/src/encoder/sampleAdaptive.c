#include "include/encoder.h"
#include "cliparser/include/cli.h"
#include "predictor/include/predictor.h"
#include <math.h>

void initSampleEncoder(uint16_t z, uint16_t * counter, uint64_t * accumulator, struct arguments * arg) {
    counter[z] = 1ULL << arg->initialY;
	accumulator[z] = ((counter[z] * ((3ULL * (1ULL << (arg->initialK+6ULL))) - 49ULL)) >> 7);
}

int encodeSampleAdaptive(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg) {

     if(x == 0 && y == 0) {
        initSampleEncoder(z, counter, accumulator, arg);
        writeBits(sampleToEncode, arg->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
    } else {
        int64_t kValue = computeLog2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = (kValue < 0) ? 0 : kValue;
        kValue = kValue > (arg->dynamicRange - 2) ? arg->dynamicRange - 2 : kValue;
        uint64_t uValue = sampleToEncode / (1<<kValue);
        if(uValue < arg->uMax) {
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
            writeBits(extractBits(sampleToEncode, kValue), kValue, numWrittenBits, totalWrittenBytes, fileToWrite);
        } else {
            writeBits(0, arg->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(sampleToEncode, arg->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        }
        if(x+y != 0) {
            if(counter[z] < ((1ULL << arg->yStar) - 1)) {
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

uint32_t decodeSampleAdaptive(uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, FILE * compressedImage, struct arguments * arg) {
    uint32_t tempSample = 0;
    if(x == 0 && y == 0) {
        initSampleEncoder(z, counter, accumulator, arg);
        tempSample = readBits(arg->dynamicRange, compressedImage);
        return tempSample;
    } else {
        int64_t kValue = computeLog2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > (arg->dynamicRange - 2) ? arg->dynamicRange - 2 : kValue;
        uint64_t uValue = 0;
        int val = 0;
        while(val != 1 && uValue < arg->uMax) {
            val = readBits(1, compressedImage);
            uValue++;
        }
        if(val == 1 && uValue > 0) {
            uValue--;
        }
        if(uValue < arg->uMax) {
            uint32_t tempBits = readBits(kValue, compressedImage);
            tempSample = (uValue *(1<<kValue)) + tempBits;
        } else {
            tempSample = readBits(arg->dynamicRange, compressedImage);
        }

        if(counter[z] < ((1 << arg->yStar) - 1)) {
                accumulator[z] += tempSample;
                counter[z]++;
            } else{
                accumulator[z] = (accumulator[z] + tempSample + 1) >> 1;
                counter[z] = (counter[z] + 1) >> 1;
        }
        return tempSample;
    }

}
