#include "include/encoder.h"
#include "cliparser/include/cli.h"
#include <stdio.h>
/*
    Extract the least significant k bits from number.
*/
long extractBits(long number, int k) { 
    return (((1 << k) - 1) & (number)); 
}

unsigned char storedValue = 0;
/*
    write #numBits of writeBits into the file stream. numWrittenBits and storedValue 
*/
void writeBits(uint16_t writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage){
    for(int i = numBits - 1; i >= 0; i--){
        storedValue |= ((writeBits >> i) & 0x1) << (7 - (*numWrittenBits));
        (*numWrittenBits)++;
        if(*numWrittenBits >= 8) {
            compressedImage[*totalWrittenBytes] = storedValue;
            *numWrittenBits = 0;
            (*totalWrittenBytes)++;
            storedValue = 0;
        }
    }
}


void fillBits(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage, struct arguments * parameters) {
    int numPaddingbits = (parameters->wordSize*8) - ((((*totalWrittenBytes)*8) + (*numWrittenBits)) % (parameters->wordSize*8));
	if(numPaddingbits < parameters->wordSize*8 && numPaddingbits > 0) {
		writeBits(0, numPaddingbits, numWrittenBits, totalWrittenBytes, compressedImage);
	}
}

/* 
    Header is structured according to chapter 5.3 in CCSDS 123 Issue 1
*/
void writeImageHeader(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage, struct arguments * parameters) {
    /* 
        Image Metadata 
    */

    // User defined Data
    writeBits(0x0, 8, numWrittenBits, totalWrittenBytes, compressedImage);
    //X Y and Z Size
    writeBits(parameters->xSize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(parameters->ySize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(parameters->zSize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    // Sample type Unsigned=0 or Signed=1
    writeBits(parameters->pixelType, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // reserved
    writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    // Dyn range
    writeBits(parameters->dynamicRange, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Sample encoding order
    if (parameters->encodeOrder != BSQ)
    {
        writeBits(0x1, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        if (parameters->encodeOrder == BIP)
        {
            // BIP: Write zSize
            writeBits(parameters->zSize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
        } else {
            //BIL: Write 1
            writeBits(0x1, 16, numWrittenBits, totalWrittenBytes, compressedImage);
        }
    } else {
        writeBits(0x1, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    }
    //Reserved
    writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    //Ouput word size
    writeBits(parameters->wordSize, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    // Entropy coder type: For this issue it is always sample adaptive
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // All zeros: Reserved of size 10 bits
    writeBits(0x0, 10, numWrittenBits, totalWrittenBytes, compressedImage);
    /* 
        PREDICTOR METADATA
    */
    // Reserved
    writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    // Num Prediction Bands
    writeBits(parameters->precedingBands, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    // Prediction mode
    writeBits(parameters->mode, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Reserved
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Local sum type: Locked to neighor oriented local sum -> 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Reserved
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Register size
    writeBits(parameters->registerSize, 6, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight component resolution
    writeBits(parameters->weightResolution-4, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Weight update scaling exponent
    writeBits(parameters->weightInterval-4, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Weight update scaling initial parameter
    writeBits(parameters->weightMin+6, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight update scaling final parameter
    writeBits(parameters->weightMax+6, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Reserved
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight init method: Always defualt -> 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight init table flag: Alaways none -> 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight init resolution: Disabled -> 5 zeros
    writeBits(0x0, 5, numWrittenBits, totalWrittenBytes, compressedImage);

    /* 
        Entropy encoder metadata: For sample adaptive
    */
    // Unary lenght limit
    writeBits(parameters->uMax, 5, numWrittenBits, totalWrittenBytes, compressedImage);
    // Rescaling counter size
    writeBits(parameters->yStar-4, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    // Initial count exponent
    writeBits(parameters->initialY, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    // Accumulator init
    writeBits(parameters->initialK, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    // Accumulator init flag -> always 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
}