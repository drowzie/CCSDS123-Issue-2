#include "include/encoder.h"
#include "cliparser/include/cli.h"
#include "predictor/include/predictor.h"
#include <stdio.h>
#include <math.h>

char activePrefix[16][258] = {"", "", "", "", "", "", "", "" , "" , "", "" , "" , "" , "", "" ,""};
uint8_t codeIndex[16] = {12, 10, 8, 6, 6, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 0};

// Internal functions
int encodeHighEntropy(uint32_t sampleToEncode, uint16_t *  counter, uint64_t * accumulator,int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg);
int encodeLowEntropy(uint64_t treshold, uint32_t sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg);


void initHybrid(uint16_t * counter, uint64_t * accumulator, int z, struct arguments * arg) {
    counter[z] = 1<<arg->initialY;
    accumulator[z] = arg->initialAccumulator;
}

int encodeHybrid(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg) {
    if(x+y == 0) {
        initHybrid(counter, accumulator, z, arg);
        writeBits(sampleToEncode, arg->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        return 0;
    }

    if(counter[z] < ((1 << arg->yStar) - 1)) {
        accumulator[z] += (uint64_t)4*sampleToEncode;
        counter[z]++;
    } else {
        writeBits(extractBits(accumulator[z], 1), 1, numWrittenBits, totalWrittenBytes, fileToWrite);
        accumulator[z] = (accumulator[z] + 4*sampleToEncode + 1) >> 1;
        counter[z] = (counter[z] + 1) >> 1;
    }
    uint64_t treshold = (accumulator[z] << 14)/counter[z];

    if (treshold > 303336) {
        encodeHighEntropy(sampleToEncode, counter, accumulator, z, totalWrittenBytes, numWrittenBits, fileToWrite, arg);
     } else {
        encodeLowEntropy(treshold, sampleToEncode, totalWrittenBytes, numWrittenBits, fileToWrite, arg);
    }
    return 0;
}


int encodeHighEntropy(uint32_t sampleToEncode, uint16_t *  counter, uint64_t * accumulator,int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg) {
    int64_t kValue = log2l((accumulator[z] + ((49*counter[z]) >> 5)) / counter[z]) - 2;
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > (arg->dynamicRange - 2) ? arg->dynamicRange - 2 : kValue;
        uint64_t uValue = sampleToEncode >> kValue;
        if(uValue < arg->uMax) {
            writeBits(extractBits(sampleToEncode, kValue), kValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
        } else {
            writeBits(sampleToEncode, arg->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(0, arg->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
        }
    return 0;
}


// Encodes values that generate X sequence
void encodeXvalues(uint32_t sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg) {
    if (sampleToEncode < arg->dynamicRange) {
        writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
        writeBits(0, sampleToEncode, numWrittenBits, totalWrittenBytes, fileToWrite);
    } else {
        writeBits(sampleToEncode, arg->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        writeBits(0, arg->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
    }
}

/*
    encodeLowEntropy will add the input character based on treshold value, and if the hash exists then it will write and reset the sequence.
 */


int encodeLowEntropy(uint64_t treshold, uint32_t sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg) {

    int index = 0;
    if (treshold <= 303336 && treshold > 225404) {
        index = 0;
    } else if (treshold <= 225404 && treshold > 166979) {
        index = 1;
    } else if (treshold <= 166979 && treshold > 128672) {
        index = 2;
    } else if (treshold <= 128672 && treshold > 95597) {
        index = 3;
    } else if (treshold <= 95597 && treshold > 69670) {
        index = 4;
    } else if (treshold <= 69670 && treshold > 50678) {
        index = 5;
    } else if (treshold <= 50678 && treshold > 34898) {
        index = 6;
    } else if (treshold <= 34898 && treshold > 23331) {
        index = 7;
    } else if (treshold <= 23331 && treshold > 14935) {
        index = 8;
    } else if (treshold <= 14935 && treshold > 9282) {
        index = 9;
    } else if (treshold <= 9282 && treshold > 5510) {
        index = 10;
    } else if (treshold <= 5510 && treshold > 3195) {
        index = 11;
    } else if (treshold <= 3195 && treshold > 1928) {
        index = 12;
    } else if (treshold <= 1928 && treshold > 1112) {
        index = 13;
    } else if (treshold <= 1112 && treshold > 408) {
        index = 14;
    } else {
        index = 15;
    }

    char input[2];
    codeWord *code = NULL;
    if(sampleToEncode > codeIndex[index]) {
        snprintf(input, 2, "X");
        encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, arg);
    } else {
        snprintf(input, 2, "%X", (char)sampleToEncode);
    }
    strncat(activePrefix[index], input, 2);
    code = searchHash(activePrefix[index], index);
    if (code) {
        writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
        activePrefix[index][0] = '\0';
    } else {
    }
    return 0;
}

// Final encoding after each
void encodeFinalStage(uint64_t * accumulator,  unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * arg) {
    codeWord *code = NULL;
    for (uint8_t i = 0; i < 16; i++) {
        code = searchFlushHash(activePrefix[i], i);
        if (code == NULL) {
            printf("Did not find a flush word for flush %d of flushword %s \n", i, activePrefix[i]);
            exit(1);
        }
        writeBits((uint64_t)code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
        activePrefix[i][0] = '\0';
    }

    for (int i = 0; i < arg->zSize; i++)
    {
        writeBits(accumulator[i], arg->dynamicRange+arg->yStar+2, numWrittenBits, totalWrittenBytes, fileToWrite);
        writeBits(1, 1, numWrittenBits, totalWrittenBytes, fileToWrite);
    }
    fillBits(numWrittenBits, totalWrittenBytes, arg, fileToWrite);
}
