#include "include/utilities.h"
#include "../cliparser/include/cli.h"
#include <stdlib.h>
#include <stdio.h>

/* 
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char fileName[128], unsigned long * samples) {
    int buffer = 0;
    FILE * sampleFile = fopen("HICO_L2_1.BSQ", "r+b");
    int readbytes = 0;
    while(fread(&buffer, 1, 2, sampleFile) == 2 && readbytes < parameters->xSize*parameters->ySize*parameters->zSize){
        //buffer = ((buffer >> 8) & 0x00FF) | ((buffer << 8) & 0xFF00);
        samples[readbytes] = buffer;
        readbytes++;
    }    
    fclose(sampleFile);
    return 0;
}
