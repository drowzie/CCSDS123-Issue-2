#include "include/encoder.h"
#include "../cliparser/include/cli.h"
#include "../predictor/include/predictor.h"
#include <stdio.h>
#include <math.h>

char activePrefix[16][258] = {"", "", "", "", "", "", "", "" , "" , "", "" , "" , "" , "", "" ,""};
uint8_t codeIndex[16] = {12, 10, 8, 6, 6, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 0};

// Internal functions
int encodeHighEntropy(uint32_t sampleToEncode, uint16_t *  counter, uint64_t * accumulator,int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
int encodeLowEntropy(uint64_t treshold, uint32_t sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);


void initHybrid(uint16_t * counter, uint64_t * accumulator, int z, struct arguments * parameters) {
    counter[z] = parameters->initialY;
    accumulator[z] = parameters->initialAccumulator;
}

int encodeHybrid(uint32_t sampleToEncode, uint16_t * counter, uint64_t * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    if(x+y == 0) {
        initHybrid(counter, accumulator, z, parameters);
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        return 0;
    }
    if(counter[z] < ((1 << parameters->yStar) - 1)) {
        accumulator[z] += 4*sampleToEncode;
        counter[z]++;
    } else {
        writeBits(extractBits(accumulator[z], 1), 1, numWrittenBits, totalWrittenBytes, fileToWrite);
        accumulator[z] = (accumulator[z] + 4*sampleToEncode + 1) >> 1;
        counter[z] = (counter[z] + 1) >> 1;
    }

    uint64_t treshold = (accumulator[z] << 14)/counter[z];
    
    if (treshold > 303336) {
        encodeHighEntropy(sampleToEncode, counter, accumulator, z, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
     } else {
        encodeLowEntropy(treshold, sampleToEncode, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
    } 
    return 0;
}


int encodeHighEntropy(uint32_t sampleToEncode, uint16_t *  counter, uint64_t * accumulator,int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    long kValue = log2((accumulator[z] + ((49*counter[z]) >> 7)) / counter[z]);
        kValue = kValue < 0 ? 0 : kValue;
        kValue = kValue > (parameters->dynamicRange - 2) ? parameters->dynamicRange - 2 : kValue;
        long uValue = sampleToEncode >> kValue;
        if(uValue < parameters->uMax) {
            writeBits(extractBits(sampleToEncode, kValue), kValue, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
            writeBits(0, uValue, numWrittenBits, totalWrittenBytes, fileToWrite);
        } else {
            writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
            writeBits(0, parameters->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
        }
}


// Encodes values that generate X sequence
void encodeXvalues(uint32_t sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    if (sampleToEncode < parameters->dynamicRange) {
        writeBits(1, 1, numWrittenBits,totalWrittenBytes, fileToWrite);
        writeBits(0, sampleToEncode, numWrittenBits, totalWrittenBytes, fileToWrite);
    } else {
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        writeBits(0, parameters->uMax, numWrittenBits, totalWrittenBytes, fileToWrite);
    }
}

/* 
    encodeLowEntropy will add the input character based on treshold value, and if the hash exists then it will write and reset the sequence.
 */

int encodeLowEntropy(uint64_t treshold, uint32_t sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    char input[2];
    codeWord *code = NULL;
    //printf("Treshold is: %ld\n", treshold);
    if (treshold <= 303336 && treshold > 225404) {
        int index = 0;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
        }
    } else if (treshold <= 225404 && treshold > 166979) {
        int index = 1;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 166979 && treshold > 128672) {
        int index = 2;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 128672 && treshold > 95597) {
        int index = 3;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 95597 && treshold > 69670) {
        int index = 4;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 69670 && treshold > 50678) {
        int index = 5;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
        }
    } else if (treshold <= 50678 && treshold > 34898) {
        int index = 6;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 34898 && treshold > 23331) {
        int index = 7;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 23331 && treshold > 14935) {
        int index = 8;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 14935 && treshold > 9282) {
        int index = 9;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 9282 && treshold > 5510) {
        int index = 10;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 5510 && treshold > 3195) {
        int index = 11;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 3195 && treshold > 1928) {
/*         int index = 12;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        } */
    } else if (treshold <= 1928 && treshold > 1112) {
        int index = 13;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    } else if (treshold <= 1112 && treshold > 408) {
        int index = 14;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        
        code = searchHash(activePrefix[index], index);
         if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        } 
    } else {
        int index = 15;
        if(sampleToEncode > codeIndex[index]) {
            snprintf(input, 2, "X");
            encodeXvalues(sampleToEncode-codeIndex[index]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            snprintf(input, 2, "%X", (char)sampleToEncode);
        }
        strncat(activePrefix[index], input, 1);
        code = searchHash(activePrefix[index], index);
        if (code) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            activePrefix[index][0] = '\0';
            
        }
    }
}

// Final encoding after each 
void encodeFinalStage(uint64_t * accumulator, int z,  unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    codeWord *code = NULL;
    for (uint8_t i = 0; i < 16; i++) {
        code = searchFlushHash(activePrefix[i], i);
        if (code == NULL) {
            // Fucky wucky
            printf("Did not find a flush word for flush %d of flushword %s \n", i, activePrefix[i]);
            exit(1);
        }
        writeBits((uint64_t)code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
        activePrefix[i][0] = '\0';
    }
    writeBits(accumulator[z], parameters->dynamicRange+parameters->yStar+2, numWrittenBits, totalWrittenBytes, fileToWrite);
    writeBits(1, 1, numWrittenBits, totalWrittenBytes, fileToWrite);
    fillBits(numWrittenBits, totalWrittenBytes, parameters, fileToWrite);
}