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
	/*
		Parse Arguments
	*/
	struct arguments arg;
	parseArguments(argc, argv, &arg);
	/*
		Mallocs related to compressions
	*/
	uint32_t * sample = malloc(arg.xSize*arg.ySize*arg.zSize*sizeof(uint32_t));
	uint32_t * sampleRep = malloc(arg.xSize*arg.ySize*arg.zSize*sizeof(uint32_t));

	/*
		Files to open for writing/reading
	*/
	FILE * residuals_file = NULL;
	FILE * decompressedFile = NULL;
	if(arg.compressionMode == DECOMPRESS) {
		decompressedFile = fopen("DecompressedFile", "wb");
		residuals_file = fopen(arg.fileName, "rb");
	} else {
		residuals_file = fopen("Encoded.bin", "wb");
	}
	unsigned int numWrittenBits = 0;
	unsigned int totalWrittenBytes = 0;
	// Read from file into sample
	if(arg.compressionMode == COMPRESS) {
		readIntSamples(&arg, arg.fileName, sample);
		writeImageHeader(&numWrittenBits, &totalWrittenBytes, residuals_file, &arg);
	} else {
		readImageHeader(residuals_file, &arg);
	}
	//Predictor
	int64_t** weights = malloc(arg.zSize*sizeof(int64_t *));
    if (weights == NULL) {
        return 1;
    }
	for (int i = 0; i < arg.zSize; i++) {
		weights[i] = calloc((arg.mode != REDUCED ? arg.precedingBands+3 : arg.precedingBands), sizeof(int64_t ));
	}

	int64_t* diffVector = calloc((arg.mode != REDUCED ? arg.precedingBands+3 : arg.precedingBands), sizeof(int64_t));
    if (diffVector == NULL) {
        return 1;
    }

	// Encoder
  	uint16_t * counter = calloc(arg.zSize, sizeof(uint16_t));
  	uint64_t * accumulator = calloc(arg.zSize, sizeof(uint64_t));
	uint32_t * decompressedSamples = malloc(arg.xSize*arg.ySize*arg.zSize*sizeof(uint32_t));
	// Error Values
	int16_t * abosluteError = calloc(arg.zSize, sizeof(int16_t));
	int16_t * relativeError = calloc(arg.zSize, sizeof(int16_t));
	for (int i = 0; i < arg.zSize; i++) {
		abosluteError[i] = arg.maximumError;
		relativeError[i] = arg.relativeError;
	}

	/*
		Begin Compression
	*/
	hashFlushCodes();
	hashCodeTableValues();

	if(arg.debugMode) {
		printParameters(&arg);
	}

	start = walltime();
	if (arg.compressionMode == COMPRESS) {
		if(arg.encodeOrder == BSQ) {
			for (uint16_t z = 0; z < arg.zSize; z++) {
				for (uint16_t y = 0; y < arg.ySize; y++) {
					for (uint16_t x = 0; x < arg.xSize; x++) {
						uint32_t tempResidual = predict(sample, x, y, z, &arg, sampleRep, diffVector, weights, abosluteError, relativeError);
						encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
						//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
					}
				}
			}
			//encodeFinalStage(accumulator, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
			fillBits(&numWrittenBits, &totalWrittenBytes, &arg, residuals_file);
		} else if (arg.encodeOrder == BIP) {
			for (uint16_t y = 0; y < arg.ySize; y++) {
				for (uint16_t x = 0; x < arg.xSize; x++) {
					for (uint16_t z = 0; z < arg.zSize; z++) {
						uint32_t tempResidual = predict(sample, x, y, z, &arg, sampleRep, diffVector, weights , abosluteError, relativeError);
						// Currently only BSQ encoding mode
						encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
						//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
					}
				}
			}
			//encodeFinalStage(accumulator, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
			fillBits(&numWrittenBits, &totalWrittenBytes, &arg, residuals_file);
		} else if (arg.encodeOrder == BIL) {
			for (uint16_t y = 0; y < arg.ySize; y++)	{
				for (uint16_t z = 0; z < arg.zSize; z++)	{
					for (uint16_t x = 0; x < arg.xSize; x++)	{
						uint32_t tempResidual = predict(sample, x, y, z, &arg, sampleRep, diffVector, weights , abosluteError, relativeError);
						encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
						//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
					}
				}
			}
		}
		//encodeFinalStage(accumulator, &totalWrittenBytes, &numWrittenBits, residuals_file, &arg);
		fillBits(&numWrittenBits, &totalWrittenBytes, &arg, residuals_file);
	} else {
		if(arg.encodeOrder == BSQ) {
			for (uint16_t z = 0; z < arg.zSize; z++) {
				for (uint16_t y = 0; y < arg.ySize; y++) {
					for (uint16_t x = 0; x < arg.xSize; x++) {
						uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, residuals_file, &arg);
						decompressedSamples[offset(x,y,z,&arg)] = unPredict(tempResidual, decompressedSamples, x, y, z, &arg, diffVector, weights, abosluteError, relativeError);
						uint32_t decompressedSample = decompressedSamples[offset(x,y,z,&arg)];
						if(arg.pixelType == SIGNED) {
							int32_t signedSample =  decompressedSample - arg.sMid;
							if(arg.endianess == BE) {
								signedSample = __builtin_bswap32(signedSample);
							}
							fwrite(&signedSample, 2, 1, decompressedFile);
						} else {
							if(arg.endianess == BE) {
								decompressedSample = __builtin_bswap32(decompressedSample);
							}
							fwrite(&decompressedSample, 2, 1, decompressedFile);
						}
					}
				}
			}
		} else if(arg.encodeOrder == BIP) {
			for (uint16_t y = 0; y < arg.ySize; y++) {
				for (uint16_t x = 0; x < arg.xSize; x++) {
					for (uint16_t z = 0; z < arg.zSize; z++) {
						uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, residuals_file, &arg);
						decompressedSamples[offset(x,y,z,&arg)] = unPredict(tempResidual, decompressedSamples, x, y, z, &arg, diffVector, weights, abosluteError, relativeError);
						uint32_t decompressedSample = decompressedSamples[offset(x,y,z,&arg)];
						if(arg.pixelType == SIGNED) {
							int32_t signedSample =  decompressedSample - arg.sMid;
							if(arg.endianess == BE) {
								signedSample = __builtin_bswap32(signedSample);
							}
							fwrite(&signedSample, 4, 1, decompressedFile);
						} else {
							if(arg.endianess == BE) {
								decompressedSample = __builtin_bswap32(decompressedSample);
							}
							fwrite(&decompressedSample, 4, 1, decompressedFile);
						}
					}
				}
			}
		} else { // BIL
			for (uint16_t y = 0; y < arg.ySize; y++)	{
				for (uint16_t z = 0; z < arg.zSize; z++)	{
					for (uint16_t x = 0; x < arg.xSize; x++)	{
						uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, residuals_file, &arg);
						decompressedSamples[offset(x,y,z,&arg)] = unPredict(tempResidual, decompressedSamples, x, y, z, &arg, diffVector, weights, abosluteError, relativeError);
						uint32_t decompressedSample = decompressedSamples[offset(x,y,z,&arg)];
						if(arg.pixelType == SIGNED) {
							int32_t signedSample =  decompressedSample - arg.sMid;
							if(arg.endianess == BE) {
								signedSample = __builtin_bswap32(signedSample);
							}
							fwrite(&signedSample, 4, 1, decompressedFile);
						} else {
							if(arg.endianess == BE) {
								decompressedSample = __builtin_bswap32(decompressedSample);
							}
							fwrite(&decompressedSample, 4, 1, decompressedFile);
						}
					}
				}
			}
		}
	}
	computeTime += walltime() - start;

	printf("\n");
	printf("Compute time:          %4.6f s\n",computeTime);
	printf("Average time per pixel(compute time/total pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(arg.xSize*arg.ySize*arg.zSize));
	printf("Average time per frame(compute time/z pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(arg.zSize));

	/*
		END OF COMPUTATION/WRITING
	*/

	fclose(residuals_file);
	// Free up stuff
	for (int i = 0; i < arg.zSize; i++)
	{
		free(weights[i]);
	}
	free(accumulator);
	free(counter);
	free(sampleRep);
	free(sample);
 	free(weights);
	free(diffVector);

	return 0;
}
