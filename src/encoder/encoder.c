#include<stdio.h>
#include "../../cliparser/include/cli.h"

int encode(unsigned int * counter, unsigned int * accumulator, int x, int y, int z, struct arguments * parameters) {

    return 0;
}

/*
    write numBits of writeValue into the file stream. numWrittenBits and storedValue 
*/

unsigned char storedValue = 0;

void writeValue(unsigned int writeValue, unsigned int numBits, unsigned int * numWrittenBits, FILE * stream){
    for(int i = numBits - 1; i >= 0; i--){
        storedValue |= ((writeValue >> i) & 0x1) << (7 - *numWrittenBits);
        (*numWrittenBits)++;
        if(*numWrittenBits >= 8){
            *numWrittenBits = 0;
            fwrite(storedValue, 1, sizeof(char), stream);
            storedValue = 0;
        }
    }
}