#include "encoder.h"
#include "cliparser/cli.h"
#include "predictor/predictor.h"
#include <math.h>

uint64_t longlog2(uint64_t x) {
    int temp = x;
    int num2 = 0;
    while (temp >>= 1) ++num2;
    return num2;
}


void initSampleEncoder(uint16_t z, uint16_t * counter, uint64_t * accumulator, struct arguments * parameters) {
    counter[z] = 1 << parameters->initialY;
	accumulator[z] = ((counter[z] * ((3 * (1 << (parameters->initialK+6))) - 49)) >> 7);
}

int encodeSampleAdaptive(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, uint8_t * compressedImage, struct arguments * parameters) {
    if(x == 0 && y == 0) {
        initSampleEncoder(z, counter, accumulator, parameters);
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, compressedImage);
    } else {
        int64_t kValue = longlog2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > (parameters->dynamicRange - 2) ? parameters->dynamicRange - 2 : kValue;
        uint64_t uValue = sampleToEncode >> kValue;
        if(uValue < parameters->uMax) {
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, compressedImage);
            writeBits(1, 1, numWrittenBits,totalWrittenBytes, compressedImage);
            writeBits(extractBits(sampleToEncode, kValue), kValue, numWrittenBits, totalWrittenBytes, compressedImage);
        } else {
            writeBits(0, parameters->uMax, numWrittenBits, totalWrittenBytes, compressedImage);
            writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, compressedImage);
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

uint32_t decodeSampleAdaptive(uint16_t * counter, uint64_t * accumulator, uint16_t x, uint16_t y, uint16_t z, FILE * compressedImage, struct arguments * parameters) {
    uint32_t tempSample = 0;
    if(x == 0 && y == 0) {
        initSampleEncoder(z, counter, accumulator, parameters);
        tempSample = readBits(parameters->dynamicRange, compressedImage);
        return tempSample;
    } else {
        int64_t kValue = longlog2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > (parameters->dynamicRange - 2) ? parameters->dynamicRange - 2 : kValue;
        
        uint64_t uValue = 0; 
        int val = 0;
        while(val != 1 && uValue < parameters->uMax) {
            val = readBits(1, compressedImage);
            uValue++;
        }
        if(val == 1 && uValue > 0) {
            uValue--;
        }
        if(uValue < parameters->uMax) {
            uint16_t tempBits = readBits(kValue, compressedImage);
            tempSample = (uValue << kValue) + tempBits;
        } else {
            tempSample = readBits(parameters->dynamicRange, compressedImage);
        }

        if(counter[z] < ((1 << parameters->yStar) - 1)) {
                accumulator[z] += tempSample;
                counter[z]++;
            } else{
                accumulator[z] = (accumulator[z] + tempSample + 1) >> 1;
                counter[z] = (counter[z] + 1) >> 1;
        }
        return tempSample;        
    }

}