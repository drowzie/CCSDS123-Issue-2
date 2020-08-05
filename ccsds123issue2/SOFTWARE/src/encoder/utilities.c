#include "include/encoder.h"
#include <stdio.h>
/*
    Extract the least significant k bits from number.
*/
uint64_t extractBits(uint64_t number, int k) {
    return (((1 << k) - 1) & (number));
}

uint8_t buffer = 0; // Active buffer
uint16_t bufferLen = 0; // Numer of bits in the buffer

/*
    Read numBits from a file
 */

uint32_t readBits(unsigned int numBits, FILE * compressedImage) {
    uint32_t readValue = 0;
    uint32_t bitsToRead = numBits;
    /*
        If we want to read more bits than what is already stored inside buffer.
     */

    while (bitsToRead > bufferLen) {
        readValue = (readValue << bufferLen) | ((buffer) >> (8 - bufferLen)); //
        bitsToRead -= bufferLen;
        fread(&buffer, 1, 1, compressedImage);
        bufferLen = 8; // Read 8 bits from storage
    }

    if(bitsToRead > 0) {
        readValue = (readValue << bitsToRead) | (buffer >> (8-bitsToRead));
        buffer = buffer << bitsToRead;
        bufferLen = bufferLen - bitsToRead;
    }
    return readValue;
}

unsigned char storedValue = 0;
/*
    write #numBits of writeBits into the file stream. numWrittenBits and storedValue
*/

void writeBits(uint64_t writeBits, uint64_t numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream){
    for(int i = numBits - 1; i >= 0; i--) {
        storedValue |= ((writeBits >> i) & 0x1) << (7 - (*numWrittenBits));
        (*numWrittenBits)++;
        if(*numWrittenBits == 8) {
            fwrite(&storedValue, 1, 1, stream);
            *numWrittenBits = 0;
            (*totalWrittenBytes)++;
            storedValue = 0;
        }
    }
}


void fillBits(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, struct arguments * arg, FILE * stream) {
    int numPaddingbits = (arg->wordSize*8) - ((((*totalWrittenBytes)*8) + (*numWrittenBits)) % (arg->wordSize*8));
	if(numPaddingbits < arg->wordSize*8 && numPaddingbits > 0) {
		writeBits(0, numPaddingbits, numWrittenBits, totalWrittenBytes, stream);
	}
}

/*
    Header is structured according to chapter 5.3 in CCSDS 123 Issue 1
*/

void writeImageHeader(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * compressedImage, struct arguments * arg) {
    /*
        Image Metadata
    */

    // User defined Data
    writeBits(0x0, 8, numWrittenBits, totalWrittenBytes, compressedImage);
    //X Y and Z Size
    writeBits(arg->xSize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(arg->ySize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(arg->zSize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
    // Sample type Unsigned=0 or Signed=1
    writeBits(arg->pixelType, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // reserved
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    if (arg->dynamicRange > 16) {
        writeBits(0x1, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    } else {
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    }
    // Dyn range
    writeBits(arg->dynamicRange, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Sample encoding order
    if (arg->encodeOrder != BSQ)
    {
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        if (arg->encodeOrder == BIP)
        {
            // BIP: Write zSize
            writeBits(arg->zSize, 16, numWrittenBits, totalWrittenBytes, compressedImage);
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
    writeBits(arg->wordSize, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    // Entropy coder type: For this issue it is always sample adaptive
    writeBits(0x1, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    // Reserved
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(arg->fidelityControl, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    // Reserved
    writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(0x0, 4, numWrittenBits, totalWrittenBytes, compressedImage);

    /*
        PREDICTOR METADATA
    */
    // Reserved
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    if(arg->sampleDamping != 0 || arg->sampleOffset != 0) {
        writeBits(0x1, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    } else {
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    }
    // Num Prediction Bands
    writeBits(arg->precedingBands, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    // Prediction mode
    writeBits(arg->mode, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight exponent offset flag -- Not used
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Local Sum type
    writeBits(arg->sumType, 2, numWrittenBits, totalWrittenBytes, compressedImage);
    writeBits(arg->registerSize, 6, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight component resolution
    writeBits(arg->weightResolution-4, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Weight update scaling exponent
    writeBits(arg->weightInterval-4, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Weight update scaling initial parameter
    writeBits(arg->weightMin+6, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight update scaling final parameter
    writeBits(arg->weightMax+6, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    //Weight update scaling initial parameter
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight init method: Always defualt -> 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight init table flag: Alaways none -> 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
    // Weight init resolution: Disabled -> 5 zeros
    writeBits(0x0, 5, numWrittenBits, totalWrittenBytes, compressedImage);

    /*
        Quantization
    */
    if(arg->fidelityControl != LOSSLESS) {
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    }
    if (arg->fidelityControl == ABSOLUTE) {
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
        uint16_t depth = arg->dynamicRange - 1 < 16 ? arg->dynamicRange - 1 : 16;
        writeBits(depth, 4, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(arg->maximumError, depth, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 8-(depth % 8), numWrittenBits, totalWrittenBytes, compressedImage);
    }
    if(arg->fidelityControl == RELATIVE) {
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 2, numWrittenBits, totalWrittenBytes, compressedImage);
        uint16_t depth = arg->dynamicRange - 1 < 16 ? arg->dynamicRange - 1 : 16;
        writeBits(depth, 4, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(arg->relativeError, depth, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 8-(depth % 8), numWrittenBits, totalWrittenBytes, compressedImage);
    }

    /*
        Sample Rep
    */
    if(arg->theta > 0) {
        writeBits(0x0, 5, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(arg->theta, 3, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        // ALl bands use the same value of damping
        writeBits(0x1, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(arg->sampleDamping, 4, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);
        writeBits(arg->sampleOffset, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    }

    /*
        Entropy encoder metadata: For sample adaptive
    */
    // Unary lenght limit
    writeBits(arg->uMax, 5, numWrittenBits, totalWrittenBytes, compressedImage);
    // Rescaling counter size
    writeBits(arg->yStar-4, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    // Initial count exponent
    writeBits(arg->initialY, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    // Accumulator init
    writeBits(arg->initialK, 4, numWrittenBits, totalWrittenBytes, compressedImage);
    // Accumulator init flag -> always 0
    writeBits(0x0, 1, numWrittenBits, totalWrittenBytes, compressedImage);

    //writeBits(arg->uMax, 5, numWrittenBits, totalWrittenBytes, compressedImage);
    //writeBits(arg->yStar-4, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    //writeBits(arg->initialY, 3, numWrittenBits, totalWrittenBytes, compressedImage);
    //writeBits(0x0, 5, numWrittenBits, totalWrittenBytes, compressedImage);

}

void readImageHeader(FILE * compressedImage, struct arguments * arg) {
    // User defined data
    readBits(8, compressedImage);
    /* IMAGE METADATA */
    arg->xSize = readBits(16, compressedImage);
    arg->ySize = readBits(16, compressedImage);
    arg->zSize = readBits(16, compressedImage);
    /* Signed type */
    arg->pixelType = readBits(1, compressedImage);
    // RESERVED
    readBits(1, compressedImage);
    if(readBits(1, compressedImage) == 1) {
        arg->dynamicRange = readBits(4, compressedImage);
        if(arg->dynamicRange == 0) {
            arg->dynamicRange = 32;
        } else {
            arg->dynamicRange = arg->dynamicRange + 16;
        }
    } else {
        arg->dynamicRange = readBits(4, compressedImage);
        if(arg->dynamicRange == 0) {
            arg->dynamicRange = 16;
        }
    }
    // Encode order
    readBits(1, compressedImage);
    uint16_t encodeOrder = readBits(16, compressedImage);
    if (encodeOrder == arg->zSize) {
        arg->encodeOrder = BIP;
        arg->imageOrder = BIP;
    } else if (encodeOrder == 0x1) {
        arg->encodeOrder = BIL;
        arg->imageOrder = BIL;
    } else {
        arg->encodeOrder = BSQ;
        arg->imageOrder = BSQ;
    }
    // RESERVED
    readBits(2, compressedImage);
    // Word size
    arg->wordSize = readBits(3, compressedImage);
    // Coder type -- Locked to sample adaptive
    readBits(2, compressedImage);
    readBits(1, compressedImage);
    arg->fidelityControl = readBits(2, compressedImage);
    readBits(2, compressedImage);
    readBits(4, compressedImage);

    // Reserved
    readBits(1, compressedImage);
    // Sample rep flag
    int sampleRepFlag = readBits(1, compressedImage);
    // Preceding bands
    arg->precedingBands = readBits(4, compressedImage);
    // Mode
    arg->mode = readBits(1, compressedImage);
    //  Weight exponent flag is always 0
    readBits(1, compressedImage);
    // Local sum mode
    arg->sumType = readBits(2, compressedImage);
    // Register size
    arg->registerSize = readBits(6, compressedImage);
    if(arg->registerSize == 0) {
        arg->registerSize = 64;
    }
    // WeightResolution
    arg->weightResolution = readBits(4, compressedImage) + 4;
    // Weight Interval
    arg->weightInterval = readBits(4, compressedImage) + 4;
    // Weight min
    arg->weightMin = readBits(4, compressedImage) - 6;
    // Weight max
    arg->weightMax = readBits(4, compressedImage) - 6;
    // Not included
    readBits(1, compressedImage);
    readBits(1, compressedImage);
    readBits(1, compressedImage);
    readBits(5, compressedImage);

    if(arg->fidelityControl != LOSSLESS) {
        readBits(1, compressedImage);
        readBits(1, compressedImage);
        readBits(2, compressedImage);
        readBits(4, compressedImage);
    }
    if(arg->fidelityControl == ABSOLUTE) {
        // Currently only absolute error limit
        readBits(1, compressedImage);
        // Always band independent
        readBits(1, compressedImage);
        // reserved
        readBits(2, compressedImage);
        // Bit depth
        uint16_t bitDepth = readBits(4, compressedImage);
        arg->maximumError = readBits(bitDepth, compressedImage);
        // Read fill bits
        readBits(8-(bitDepth % 8), compressedImage);
    }
    if(arg->fidelityControl == RELATIVE) {
        // Currently only absolute error limit
        readBits(1, compressedImage);
        // Always band independent
        readBits(1, compressedImage);
        // reserved
        readBits(2, compressedImage);
        // Bit depth
        uint16_t bitDepth = readBits(4, compressedImage);
        arg->relativeError = readBits(bitDepth, compressedImage);
        // Read fill bits
        readBits(8-(bitDepth % 8), compressedImage);
    }

    /*
        Sample rep
    */
    if(sampleRepFlag != 0) {
        // Sample rep
        readBits(5, compressedImage);
        arg->theta = readBits(3, compressedImage);
        readBits(1, compressedImage);
        // All bands use same value -> always 0
        readBits(1, compressedImage);
        readBits(1, compressedImage);
        readBits(1, compressedImage);
        // Value of damping
        arg->sampleDamping = readBits(4, compressedImage);
        readBits(1, compressedImage);
        readBits(1, compressedImage);
        readBits(1, compressedImage);
        readBits(1, compressedImage);
        arg->sampleOffset = readBits(4, compressedImage);
    }

    /*
        Entropy encoder metadata: For sample adaptive
    */
    arg->uMax = readBits(5, compressedImage);
    if(arg->uMax == 0) {
        arg->uMax = 32;
    }

    // Y star
    arg->yStar = readBits(3, compressedImage) + 4;
    // Initial y
    arg->initialY = readBits(3, compressedImage);
    if(arg->initialY == 0) {
        arg->initialY = 8;
    }
    // Init k
    arg->initialK = readBits(4, compressedImage);
    // Acc init flag
    readBits(1, compressedImage);
    // arg defined by what is read in the file
    arg->sMin = 0;
    arg->sMax = (1ULL << arg->dynamicRange) - 1;
    arg->sMid = 1ULL << (arg->dynamicRange - 1);



}


uint64_t computeLog2 (uint64_t x) {
    int num2s = 0;
    while (x >>= 1) ++num2s;
    return num2s;
}
