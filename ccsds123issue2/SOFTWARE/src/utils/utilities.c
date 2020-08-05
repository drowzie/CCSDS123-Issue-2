#include "include/utilities.h"
#include "cliparser/include/cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "predictor/include/predictor.h"
/*
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char fileName[128], uint32_t * samples) {
    int32_t buffer = 0;
    FILE * sampleFile = fopen(fileName, "r+b");
    uint64_t readbytes = 0;
    int bytestoread = parameters->pixelSize / 8;
    while(fread(&buffer, 1, bytestoread, sampleFile) == bytestoread){
      // Big endian to Little Endian
      if(parameters->endianess == BE) {
        buffer = __builtin_bswap16(buffer);
      }
  		if(parameters->pixelType == SIGNED) {
        samples[readbytes] = buffer + parameters->sMid;
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
