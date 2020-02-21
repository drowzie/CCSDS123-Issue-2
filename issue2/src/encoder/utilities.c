#include "include/encoder.h"
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

void writeBits(uint64_t writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream){
    for(int i = numBits - 1; i >= 0; i--){
        storedValue |= ((writeBits >> i) & 0x1) << (7 - (*numWrittenBits));
        (*numWrittenBits)++;
        if(*numWrittenBits >= 8) {
            fwrite(&storedValue, 1, 1, stream);
            *numWrittenBits = 0;
            (*totalWrittenBytes)++;
            storedValue = 0;
        }
    }
}


void fillBits(unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, struct arguments * parameters, FILE * stream) {
    int numPaddingbits = (parameters->wordSize*8) - ((((*totalWrittenBytes)*8) + (*numWrittenBits)) % (parameters->wordSize*8));
	if(numPaddingbits < parameters->wordSize*8 && numPaddingbits > 0) {
		writeBits(0, numPaddingbits, numWrittenBits, totalWrittenBytes, stream);
	}
}