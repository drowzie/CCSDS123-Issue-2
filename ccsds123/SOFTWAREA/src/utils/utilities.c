#include "utilities.h"
#include "cliparser/cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "predictor/predictor.h"

/* 
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char fileName[128], uint16_t * samples) {
    int16_t buffer = 0;
    FILE * sampleFile = fopen(fileName, "r+b");
    uint32_t readbytes = 0;
    while(fread(&buffer, 1, 2, sampleFile) == 2 && readbytes < parameters->xSize*parameters->ySize*parameters->zSize){
		if(parameters->pixelType == SIGNED) {
			buffer = (buffer + parameters->sMid);
            samples[readbytes] = buffer;
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
