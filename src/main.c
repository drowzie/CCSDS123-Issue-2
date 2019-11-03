
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h> 

#include "cliparser/include/cli.h"
#include "predictor/include/predictor.h"
#include "encoder/include/encoder.h"
#include "utils/include/utilities.h"


void insertTestData(unsigned int * sample, struct arguments * args){
	sample[offset(0,0,0, args)] = 4;
	sample[offset(1,0,0, args)] = 4;
	sample[offset(2,0,0, args)] = 4;
	sample[offset(3,0,0, args)] = 9;

	sample[offset(0,1,0, args)] = 3;
	sample[offset(1,1,0, args)] = 4;
	sample[offset(2,1,0, args)] = 5;
	sample[offset(3,1,0, args)] = 5;

	sample[offset(0,2,0, args)] = 6;
	sample[offset(1,2,0, args)] = 7;
	sample[offset(2,2,0, args)] = 7;
	sample[offset(3,2,0, args)] = 8;
 
	sample[offset(0,3,0, args)] = 6;
	sample[offset(1,3,0, args)] = 7;
	sample[offset(2,3,0, args)] = 7;
	sample[offset(3,3,0, args)] = 7;

	//
	sample[offset(0,0,1, args)] = 3;
	sample[offset(1,0,1, args)] = 4;
	sample[offset(2,0,1, args)] = 5;
	sample[offset(3,0,1, args)] = 5;

	sample[offset(0,1,1, args)] = 6;
	sample[offset(1,1,1, args)] = 2;
	sample[offset(2,1,1, args)] = 1;
	sample[offset(3,1,1, args)] = 1;

	sample[offset(0,2,1, args)] = 3;
	sample[offset(1,2,1, args)] = 4;
	sample[offset(2,2,1, args)] = 5;
	sample[offset(3,2,1, args)] = 5;

	sample[offset(0,3,1, args)] = 3;
	sample[offset(1,3,1, args)] = 4;
	sample[offset(2,3,1, args)] = 5;
	sample[offset(3,3,1, args)] = 5;
}

void writearary(unsigned int * sample, struct arguments * args) {
    FILE * file = fopen("test.bin", "w+b");

	for (int z = 0; z < args->zSize; z++)
	{
		for (int y = 0; y < args->ySize; y++)
		{
			for (int x = 0; x < args->xSize; x++)
			{
				fwrite(&sample[offset(x,y,z,args)], 1, 2, file);
			}
			
		}
		
	}
	

	fclose(file);
}


int main(int argc, char **argv)
{
	/*
		Parse arguments. --h to see what available arguments(declared in cli.c)
	*/

	struct arguments parameters;
	parseArguments(argc, argv, &parameters);

	long  sMin = 0;
    long  sMax = (0x1 << parameters.dynamicRange) - 1;
    long  sMid = 0x1 << (parameters.dynamicRange - 1);
	printf("sMax %ld,SMin %ld,smid %ld \n", sMax, sMin, sMid);
	// IMAGE	
	unsigned int * sample = (int*) malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(unsigned int));

	/* 
		PREDICTION SPECIFIC MALLOCS
	*/
	unsigned long * residuals = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(unsigned long));
	int * localsum = (int*) calloc(parameters.xSize*parameters.ySize*parameters.zSize, sizeof(int));
	unsigned int * sampleRep = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(unsigned int));

	/* 
		2D Array structured as:
		I:0 = # Preceding bands
		IF FULL VECTOR IS USED:
		I:1 = N Vector
		I:1 = W Vector
		I:1 = NW Vector
	 */
	long ** weights = malloc((parameters.mode != REDUCED ? 4 : 1) * sizeof(long *));
    for (int i=0; i<(parameters.mode != REDUCED ? 4 : 1); i++) {
         weights[i] = calloc((i == 0 ? parameters.precedingBands : 1), sizeof(long));
    }
	int ** diffVector = malloc((parameters.mode != REDUCED ? 4 : 1) * sizeof(int *));
    for (int i=0; i<(parameters.mode != REDUCED ? 4 : 1); i++) { 
         diffVector[i] = calloc((i == 0 ? parameters.precedingBands : 1), sizeof(int)); 
	}
	/* 
		ENCODING SPECIFIC MALLOCS
	*/
    unsigned int * counter = malloc(sizeof(unsigned int)*parameters.zSize);
    unsigned int * accumulator = malloc(sizeof(unsigned int)*parameters.zSize);

	
	FILE * residuals_file = NULL;
	residuals_file = fopen("Encoded.bin", "w+b");
	FILE * deltafile = fopen("Encoded.bin.delta", "w+b");
	//insertTestData(sample, &parameters);
	//writearary(sample, &parameters);

	unsigned int numWrittenBits = 0;
	unsigned int totalWrittenBytes = 0;

	/* 
		ACTUAL COMPUTATION/WRITING
	*/
	printf("Started reading\n");
	char filename[128] = "test.bin";
	readIntSamples(&parameters, filename, sample);

 	for (int z = 0; z < parameters.zSize; z++) {
		counter[z] = 0x1 << parameters.initialY;
		accumulator[z] = ((counter[z] * ((3 * (1 << (parameters.initialK+6))) - 49)) >> 7);
		for (int y = 0; y < parameters.ySize; y++) {
			for (int x = 0; x < parameters.xSize; x++) {
				predict(sample, residuals, x, y, z, &parameters, sampleRep, localsum, diffVector, weights, sMin, sMax, sMid, 0, 0, 0);
				// Currently only BSQ encoding mode
				fwrite(&residuals[offset(x,y,z,&parameters)], 1, sizeof(unsigned int), deltafile);
				encodeSampleAdaptive(residuals[offset(x,y,z,&parameters)], counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
			}
		}
	}

	printArrayInt(sample, &parameters);
	//printArrayInt(sampleRep, &parameters);
	printArrayLong(residuals, &parameters);

	// Fill up the rest to fill up word size
 	int numPaddingbits = (parameters.wordSize*8) - (((totalWrittenBytes*8) + numWrittenBits) % (parameters.wordSize*8));
	if(numPaddingbits < parameters.wordSize*8 && numPaddingbits > 0) {
		writeBits(0, numPaddingbits, &numWrittenBits, &totalWrittenBytes, residuals_file);
	} 

	/* 
		END OF COMPUTATION/WRITING
	*/

	fclose(residuals_file);
	fclose(deltafile);
	//printArrayInt(sample,&parameters);
	//printArrayLong(residuals, &parameters);

	// Free up stuff
	free(accumulator);
	free(counter);
	free(sampleRep);
	free(sample);
	free(residuals);
	free(localsum);

    for (int i=0; i<(parameters.mode != REDUCED ? 4 : 1); i++) {
         free(weights[i]);
    }

    for (int i=0; i<(parameters.mode != REDUCED ? 4 : 1); i++) { 
         free(diffVector[i]);
    }
 	free(weights);
	free(diffVector); 

	return 0;
}
