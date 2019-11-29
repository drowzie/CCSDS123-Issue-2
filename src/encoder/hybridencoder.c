#include "include/encoder.h"
#include "../cliparser/include/cli.h"
#include "../predictor/include/predictor.h"
#include <math.h>

char activePrefix[16][256];
uint8_t codeIndex[16] = {12, 10, 8, 6, 6, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 0};

// Internal functions
int encodeHighEntropy(unsigned long sampleToEncode, uint32_t *  counter, uint32_t * accumulator,int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
int encodeLowEntropy(uint32_t treshold, unsigned long sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters);
/* 
    END OF HEADER
*/

int encodeHybrid(unsigned long sampleToEncode, uint32_t * counter, uint32_t * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    if(x+y == 0) {
        writeBits(sampleToEncode, parameters->dynamicRange, numWrittenBits, totalWrittenBytes, fileToWrite);
        return 0;
    }

    if(counter[z] < ((1 << parameters->yStar) - 1)) {
        accumulator[z] += 4*sampleToEncode;
        counter[z]++;
    } else{
        accumulator[z] = (accumulator[z] + 4*sampleToEncode + 1) >> 1;
        counter[z] = (counter[z] + 1) >> 1;
        writeBits(extractBits(accumulator[z], 1), 1, numWrittenBits, totalWrittenBytes, fileToWrite);
    }

    int treshold = 0;

    if (treshold > 303336) {
        encodeHighEntropy(sampleToEncode, counter, accumulator, z, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
    } else {
        encodeLowEntropy(treshold, sampleToEncode, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
    }

    return 0;
}


int encodeHighEntropy(unsigned long sampleToEncode, uint32_t *  counter, uint32_t * accumulator,int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
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
void encodeXvalues(unsigned long sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
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
int encodeLowEntropy(uint32_t treshold, unsigned long sampleToEncode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {
    char input;
    if (treshold <= 303336 && treshold > 225404) {
        if(sampleToEncode > codeIndex[15]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[15]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[15], &input);
        codeWord *code = searchHash(activePrefix[15], 15);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[15], "");
        }
    } else if (treshold <= 225404 && treshold > 166979) {
        if(sampleToEncode > codeIndex[14]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[14]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[14], &input);
        codeWord *code = searchHash(activePrefix[14], 14);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[14], "");
        }
    } else if (treshold <= 166979 && treshold > 128672) {
        if(sampleToEncode > codeIndex[13]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[13]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[13], &input);
        codeWord *code = searchHash(activePrefix[13], 13);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[13], "");
        }
    } else if (treshold <= 128672 && treshold > 95597) {
        if(sampleToEncode > codeIndex[12]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[12]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[12], &input);
        codeWord *code = searchHash(activePrefix[12], 12);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[12], "");
        }
    } else if (treshold <= 95597 && treshold > 69670) {
        if(sampleToEncode > codeIndex[11]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[11]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[11], &input);
        codeWord *code = searchHash(activePrefix[11], 11);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[11], "");
        }
    } else if (treshold <= 69670 && treshold > 50678) {
        if(sampleToEncode > codeIndex[10]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[10]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[10], &input);
        codeWord *code = searchHash(activePrefix[10], 10);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[10], "");
        }
    } else if (treshold <= 50678 && treshold > 34898) {
        if(sampleToEncode > codeIndex[9]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[9]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[9], &input);
        codeWord *code = searchHash(activePrefix[9], 9);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[9], "");
        }
    } else if (treshold <= 34898 && treshold > 23331) {
        if(sampleToEncode > codeIndex[8]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[8]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[8], &input);
        codeWord *code = searchHash(activePrefix[8], 8);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[8], "");
        }
    } else if (treshold <= 23331 && treshold > 14935) {
        if(sampleToEncode > codeIndex[7]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[7]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[7], &input);
        codeWord *code = searchHash(activePrefix[7], 7);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[7], "");
        }
    } else if (treshold <= 14935 && treshold > 9282) {
        if(sampleToEncode > codeIndex[6]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[6]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[6], &input);
        codeWord *code = searchHash(activePrefix[6], 6);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[6], "");
        }
    } else if (treshold <= 9282 && treshold > 5510) {
        if(sampleToEncode > codeIndex[5]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[5]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[5], &input);
        codeWord *code = searchHash(activePrefix[5], 5);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[5], "");
        }
    } else if (treshold <= 5510 && treshold > 3195) {
        if(sampleToEncode > codeIndex[4]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[4]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[4], &input);
        codeWord *code = searchHash(activePrefix[4], 4);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[4], "");
        }
    } else if (treshold <= 3195 && treshold > 1928) {
        if(sampleToEncode > codeIndex[3]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[3]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[3], &input);
        codeWord *code = searchHash(activePrefix[3], 3);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[3], "");
        }
    } else if (treshold <= 1928 && treshold > 1112) {
        if(sampleToEncode > codeIndex[2]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[2]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[2], &input);
        codeWord *code = searchHash(activePrefix[2], 2);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[2], "");
        }
    } else if (treshold <= 1112 && treshold > 408) {
        if(sampleToEncode > codeIndex[1]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[1]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[1], &input);
        codeWord *code = searchHash(activePrefix[1], 1);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[1], "");
        }
    } else {
        if(sampleToEncode > codeIndex[0]) {
            input = 'X';
            encodeXvalues(sampleToEncode-codeIndex[0]-1, totalWrittenBytes, numWrittenBits, fileToWrite, parameters);
        } else {
            input = (char)sampleToEncode;
        }
        strcat(activePrefix[0], &input);
        codeWord *code = searchHash(activePrefix[0], 0);
        if (code != NULL) {
            writeBits(code->codeWordValue, code->bitSize, numWrittenBits, totalWrittenBytes, fileToWrite);
            sprintf(activePrefix[0], "");
        }
    }
}