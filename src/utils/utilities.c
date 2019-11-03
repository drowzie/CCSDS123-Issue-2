#include "include/utilities.h"
#include "../cliparser/include/cli.h"
#include <stdlib.h>
#include <stdio.h>

/* 
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char fileName[128], unsigned int * samples) {
    int buffer = 0;
    FILE * sampleFile = fopen("test.bin", "rb");
    int readbytes = 0;
    while(fread(&buffer, 1, 2, sampleFile) == 2){
        //buffer = ((buffer >> 8) & 0x00FF) | ((buffer << 8) & 0xFF00);
        samples[readbytes] = buffer;
        readbytes++;
    }    
    fclose(sampleFile);
}
