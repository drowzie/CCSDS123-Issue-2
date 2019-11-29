#include "include/encoder.h"
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
void writeBits(unsigned long writeBits, unsigned int numBits, unsigned int * numWrittenBits, unsigned int * totalWrittenBytes, FILE * stream){
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