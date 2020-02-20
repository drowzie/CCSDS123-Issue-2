#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "cliparser/include/cli.h"
#include "predictor/include/predictor.h"
#include "encoder/include/encoder.h"
#include "utils/include/utilities.h"
#include <sys/time.h>

int main(int argc, char **argv) {
	double start;
	double computeTime=0;
  	double writeTime=0;
	/* 
		Parse Arguments 
	*/ 
	struct arguments parameters;
	parseArguments(argc, argv, &parameters);
	/*
		Mallocs related to compressions
	*/
	uint16_t * sample = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint16_t));
	//Predictor
	int32_t * weights = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(int32_t ));
	int32_t * diffVector = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(int32_t ));
	// Encoder
    uint16_t * counter = malloc(sizeof(uint16_t)*parameters.zSize);
    uint64_t * accumulator = malloc(sizeof(uint64_t)*parameters.zSize);
	// Output compressed image 
	uint8_t * compressedImage = malloc(((parameters.dynamicRange + 7)/8)*parameters.xSize*parameters.ySize*parameters.zSize);

	// Files to write 
	FILE * deltafile = NULL;
	if(parameters.debugMode) {
		deltafile = fopen("Encoded.bin.delta", "w+b");
	}
	FILE * residuals_file = fopen("Encoded.bin", "w+b");

	unsigned int numWrittenBits = 0;
	unsigned int totalWrittenBytes = 0;
	readIntSamples(&parameters, parameters.fileName, sample);

	printf("Computing: \n Encode order: %d Offset order: %d \n", parameters.encodeOrder, parameters.imageOrder);
	start = walltime();
	/* 
		START OF COMPRESSION
	*/
	// Add the image header
	writeImageHeader(&numWrittenBits, &totalWrittenBytes, compressedImage, &parameters);

	if(parameters.encodeOrder == BSQ) {
		for (uint16_t z = 0; z < parameters.zSize; z++) {
			for (uint16_t y = 0; y < parameters.ySize; y++) {
				for (uint16_t x = 0; x < parameters.xSize; x++) {
					uint32_t residuals = (uint32_t)predict(sample, x, y, z, &parameters, diffVector, weights);
					if(parameters.debugMode) {
						fwrite((&residuals), 4, 1, deltafile);
					}
					//Currently only BSQ encoding mode
					encodeSampleAdaptive(residuals, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, compressedImage, &parameters);
				}
			}
		}
	} 
	else if (parameters.encodeOrder == BIP) {
		for (uint16_t y = 0; y < parameters.ySize; y++) {
			for (uint16_t x = 0; x < parameters.xSize; x++) {
				for (uint16_t z = 0; z < parameters.zSize; z++) {
					uint32_t residuals = predict(sample, x, y, z, &parameters, diffVector, weights);
					// Currently only BSQ encoding mode
					if(parameters.debugMode) {
						fwrite((&residuals), 4, 1, deltafile);
					}
					encodeSampleAdaptive(residuals, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, compressedImage, &parameters);
				}
			}
		}
	} else if (parameters.encodeOrder == BIL) {
		for (uint16_t y = 0; y < parameters.ySize; y++)	{
			for (uint16_t z = 0; z < parameters.zSize; z++)	{
				for (uint16_t x = 0; x < parameters.xSize; x++)	{
					uint32_t residuals = predict(sample, x, y, z, &parameters, diffVector, weights);
					// Currently only BSQ encoding mode
					if(parameters.debugMode) {
						fwrite((&residuals), 4, 1, deltafile);
					}
					encodeSampleAdaptive(residuals, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, compressedImage, &parameters);
				}
			}
		}
	}

	/* 
		Fill the final bits to reach the word size of the computer
	*/
	fillBits(&numWrittenBits, &totalWrittenBytes, compressedImage, &parameters);
	computeTime += walltime() - start;

	//Finally store the compressed image
	fwrite(compressedImage, 1, totalWrittenBytes, residuals_file);


	/* 
		END OF COMPRESSION
	*/

	printf("\n");
	printf("Compute time:          %4.6f s\n",computeTime);
	printf("Average time per pixel(compute time/total pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(parameters.xSize*parameters.ySize*parameters.zSize));
	printf("Average time per frame(compute time/z pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(parameters.zSize));

	/*
		END OF COMPUTATION/WRITING
	*/

	fclose(residuals_file);
	if (parameters.debugMode)
	{
		fclose(deltafile);
	}
	
	// Free up stuff
	free(accumulator);
	free(counter);
	free(sample);
 	free(weights);
	free(diffVector);

	return 0;
}
