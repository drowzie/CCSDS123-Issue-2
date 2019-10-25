#include "include/utils.h"
#include "../cliparser/include/cli.h"
#include <stdlib.h>
#include <stdio.h>

/* 
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char * fileName, unsigned int * samples) {
    unsigned int buffer = 0;
    FILE * sampleFile = fopen(fileName, "r+b");
    int readelements = 0;
    while(fread(&buffer, 1, 2, sampleFile) == 2 && readelements < parameters->xSize*parameters->ySize*parameters->zSize ){
        
    }

}