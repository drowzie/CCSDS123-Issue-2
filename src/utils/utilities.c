#include "include/utilities.h"
#include "../cliparser/include/cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

/* 
    Read Integer samples into memory
 */
int readIntSamples(struct arguments * parameters, char fileName[128], unsigned long * samples) {
    int buffer = 0;
    FILE * sampleFile = fopen(fileName, "r+b");
    int readbytes = 0;
    while(fread(&buffer, 1, 2, sampleFile) == 2 && readbytes < parameters->xSize*parameters->ySize*parameters->zSize){
        //buffer = ((buffer >> 8) & 0x00FF) | ((buffer << 8) & 0xFF00);
        samples[readbytes] = buffer;
        readbytes++;
    }    
    fclose(sampleFile);
    return 0;
}


void printArrayLong(unsigned long * sample, struct arguments * args) {
	for (int z = 0; z < args->zSize; z++)
	{
		printf("Z=%d\n\n", z);
		for (int y = 0; y < args->ySize; y++)
		{
			for (int x = 0; x < args->xSize; x++)
			{
				printf("%lu ", sample[offset(x,y,z,args)]);
			}
			printf("\n");
		}
	}
}

void writeArrayLong(unsigned long * sample, FILE * file, struct arguments * args) {
	for (int z = 0; z < args->zSize; z++)
	{
		for (int y = 0; y < args->ySize; y++)
		{
			for (int x = 0; x < args->xSize; x++)
			{
                long residual = sample[offset(x,y,z,args)];
				fwrite(&residual, 1, 2, file);
			}
		}
	}
}

double walltime ( void ) {
	static struct timeval t;
	gettimeofday ( &t, NULL );
	return ( t.tv_sec + 1e-6 * t.tv_usec );
}
