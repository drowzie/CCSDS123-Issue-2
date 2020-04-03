#include "encoder.h"
#include "cliparser/cli.h"
#include <stdio.h>
/*
    Extract the least significant k bits from number.
*/
long extractBits(long number, int k) { 
    return (((1 << k) - 1) & (number)); 
}

uint8_t buffer = 0; // Active buffer
uint16_t bufferLen = 0; // Numer of bits in the buffer

uint16_t readNZeros(uint16_t maxBits, FILE * compressedImage) {
    uint16_t count = 0;
    while (count < maxBits) {
        if(bufferLen == 0) {
            fread(&buffer, 1, 1, compressedImage);
            bufferLen = 8;
        }
        bufferLen = bufferLen - 1;
        if((buffer & 0x80) != 0) {
            buffer = buffer << 1;
            return count;
        } else {
            buffer = buffer << 1;
            count++;
        }
    }
    return maxBits + 1;
}

/* 
    Read numBits from a file
 */
uint16_t readBits(unsigned int numBits, FILE * compressedImage) {
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
void writeBits(uint16_t writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, uint8_t * compressedImage){
    for(int i = numBits - 1; i >= 0; i--){
        storedValue |= ((writeBits >> i) & 0x1) << (7 - (*numWrittenBits));
        (*numWrittenBits)++;
        if(*numWrittenBits >= 8) {
            // Inside here means a byte has been written. Can change to fwrite a byte or send it on network.
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

/* 
    Header is structured according to chapter 5.3 in CCSDS 123 Issue 1
*/
void readImageHeader(FILE * compressedImage, struct arguments * parameters) {
    // User defined data
    readBits(8, compressedImage);
    /* IMAGE METADATA */
    parameters->xSize = readBits(16, compressedImage);
    parameters->ySize = readBits(16, compressedImage);
    parameters->zSize = readBits(16, compressedImage);
    /* Signed type */
    parameters->pixelType = readBits(1, compressedImage);
    //RESERVED
    readBits(2, compressedImage);
    // DYN RANGE
    parameters->dynamicRange = readBits(4, compressedImage);
    if(parameters->dynamicRange == 0) {
        parameters->dynamicRange = 16;
    }
    // Encode order
    readBits(1, compressedImage);
    uint16_t encodeOrder = readBits(16, compressedImage);
    if (encodeOrder == parameters->zSize) {
        parameters->encodeOrder = BIP;
        parameters->imageOrder = BIP;
    } else if (encodeOrder == 0x1) {
        parameters->encodeOrder = BIL;
        parameters->imageOrder = BIL;
    } else {
        parameters->encodeOrder = BSQ;
        parameters->imageOrder = BSQ;
    }
    // RESERVED
    readBits(2, compressedImage);
    // Word size
    parameters->wordSize = readBits(3, compressedImage);
    // Coder type -- Locked to sample adaptive
    readBits(1, compressedImage);
    // Reserved
    readBits(10, compressedImage);
    /* Prediction metadata */
    // Reserved
    readBits(2, compressedImage);
    // Preceding bands
    parameters->precedingBands = readBits(4, compressedImage);
    // Mode
    parameters->mode = readBits(1, compressedImage);
    // Reserved
    readBits(1, compressedImage);
    // Local sum mode, locked to neighbor
    readBits(1, compressedImage);
    // Reserved
    readBits(1, compressedImage);
    // Register size
    parameters->registerSize = readBits(6, compressedImage);
    // WeightResolution
    parameters->weightResolution = readBits(4, compressedImage) + 4;
    // Weight Interval
    parameters->weightInterval = readBits(4, compressedImage) + 4;
    // Weight min
    parameters->weightMin = readBits(4, compressedImage) - 6;
    // Weight max
    parameters->weightMax = readBits(4, compressedImage) - 6;
    // Reserved
    readBits(1, compressedImage);
    // Custom weight init is disabled for this implementation
    // Weight init method
    readBits(1, compressedImage);
    // Weight init table
    readBits(1, compressedImage);
    // Weight init resolution 
    readBits(5, compressedImage);
    /* 
        Entropy encoder metadata: For sample adaptive
    */
    parameters->uMax = readBits(5, compressedImage);
    // Y star
    parameters->yStar = readBits(3, compressedImage) + 4;
    // Initial y
    parameters->initialY = readBits(3, compressedImage);
    // Init k
    parameters->initialK = readBits(4, compressedImage);
    // Acc init flag
    readBits(1, compressedImage);
    // Parameters defined by what is read in the file
    parameters->sMin = 0;
    parameters->sMax = (0x1 << parameters->dynamicRange) - 1;
    parameters->sMid = 0x1 << (parameters->dynamicRange - 1);
}