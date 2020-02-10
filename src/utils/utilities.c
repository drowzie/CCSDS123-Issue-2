#include "include/utilities.h"
#include "../cliparser/include/cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "../predictor/include/predictor.h"

/* 
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char fileName[128], uint32_t * samples) {
    int16_t buffer = 0;
    FILE * sampleFile = fopen(fileName, "r+b");
    int32_t readbytes = 0;
	uint32_t signMask = 0xFFFF << parameters->dynamicRange;
    while(fread(&buffer, 1, 2, sampleFile) == 2 && readbytes < parameters->xSize*parameters->ySize*parameters->zSize){
        //buffer = ((buffer >> 8) & 0x00FF) | ((buffer << 8) & 0xFF00);
		//buffer |= signMask;
		if(parameters->pixelType == SIGNED) {
			samples[readbytes] = (uint16_t) (buffer + parameters->sMid); // Signed to unsigned
		} else {
			samples[readbytes] = buffer;
		}
        readbytes++;
    }    
    fclose(sampleFile);
    return 0;
}

double walltime ( void ) {
	static struct timeval t;
	gettimeofday ( &t, NULL );
	return ( t.tv_sec + 1e-6 * t.tv_usec );
}
