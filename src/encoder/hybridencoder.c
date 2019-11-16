#include "include/encoder.h"
#include "../cliparser/include/cli.h"
#include "../predictor/include/predictor.h"
#include <math.h>


int encodeHybrid(unsigned long sampleToEncode, unsigned int * counter, unsigned int * accumulator, int x, int y, int z, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite, struct arguments * parameters) {



    return 0;
}



int writeCodeWord0(int inputCode, unsigned int * totalWrittenBytes, unsigned int * numWrittenBits, FILE * fileToWrite) {
    int outputCode = 0;
    switch (inputCode) {
    case 0x00:
        writeBits(0x19, 5, numWrittenBits, totalWrittenBytes, fileToWrite);
        break;
    default:
        break;
    }


    return 0;
}