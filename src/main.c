
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

	/* 
		LOSSLESS Compression: 
	 */

	long  sMin = 0;
    long  sMax = (0x1 << parameters.dynamicRange) - 1;
    long  sMid = 0x1 << (parameters.dynamicRange - 1);
	printf("sMax %ld,SMin %ld,smid %ld \n", sMax, sMin, sMid);
	// IMAGE	
	unsigned long * sample = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(unsigned long));
	/* 
		PREDICTION SPECIFIC MALLOCS
	*/
	long * localsum = malloc(parameters.xSize*parameters.ySize*parameters.zSize* sizeof(long));
	unsigned long * sampleRep = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(unsigned long));

	long * weights = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(long ));
	long * diffVector = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(long ));
	/* 
		ENCODING SPECIFIC MALLOCS
	*/
    unsigned int * counter = malloc(sizeof(unsigned int)*parameters.zSize);
    unsigned int * accumulator = malloc(sizeof(unsigned int)*parameters.zSize);

	
	FILE * residuals_file = NULL;
	residuals_file = fopen("Encoded.bin", "w+b");
	FILE * deltafile = fopen("Encoded.bin.delta", "wb");
	//insertTestData(sample, &parameters);
	//writearary(sample, &parameters);

	unsigned int numWrittenBits = 0;
	unsigned int totalWrittenBytes = 0;

	/* 
		ACTUAL COMPUTATION/WRITING
	*/
	printf("Started reading\n");
	char filename[128] = "HICO_L2_1.BSQ";
	readIntSamples(&parameters, filename, sample);

 	for (int z = 0; z < parameters.zSize; z++) {
		counter[z] = 1 << parameters.initialY;
		accumulator[z] = ((counter[z] * ((3 * (1 << (parameters.initialK+6))) - 49)) >> 7);
		for (int y = 0; y < parameters.ySize; y++) {
			for (int x = 0; x < parameters.xSize; x++) {
				unsigned long tempResidual = predict(sample, x, y, z, &parameters, sampleRep, localsum, diffVector, weights, sMin, sMax, sMid, 0, 0, 0, 0, 0);
				// Currently only BSQ encoding mode
				fwrite((&tempResidual), 1, sizeof(unsigned int), deltafile);
				encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
			}
		}
	}

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

	// Free up stuff
	free(accumulator);
	free(counter);
	free(sampleRep);
	free(sample);
	free(localsum);
 	free(weights);
	free(diffVector); 

	return 0;
}
