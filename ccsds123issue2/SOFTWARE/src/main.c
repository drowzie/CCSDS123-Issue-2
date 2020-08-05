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
	struct arguments parameters;
	parseArguments(argc, argv, &parameters);
	/*
		Mallocs related to compressions
	*/
	uint32_t * sample = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint32_t));
	uint32_t * sampleRep = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint32_t));
	
	/*
		Files to open for writing/reading
	*/
	FILE * residuals_file = NULL;
	FILE * decompressedFile = NULL;
	if(parameters.compressionMode == DECOMPRESS) {
		decompressedFile = fopen("DecompressedFile", "wb");
		residuals_file = fopen(parameters.fileName, "rb");
	} else {
		residuals_file = fopen("Encoded.bin", "wb");
	}
	unsigned int numWrittenBits = 0;
	unsigned int totalWrittenBytes = 0;
	// Read from file into sample
	if(parameters.compressionMode == COMPRESS) {
		readIntSamples(&parameters, parameters.fileName, sample);
		writeImageHeader(&numWrittenBits, &totalWrittenBytes, residuals_file, &parameters);
	} else {
		readImageHeader(residuals_file, &parameters);
	}
	//Predictor
	int64_t** weights = malloc(parameters.zSize*sizeof(int64_t *));
    if (weights == NULL) {
        return 1;
    }
	for (int i = 0; i < parameters.zSize; i++) {
		weights[i] = calloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands), sizeof(int64_t ));
	}

	int64_t* diffVector = calloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands), sizeof(int64_t));
    if (diffVector == NULL) {
        return 1;
    }

	// Encoder
  	uint16_t * counter = calloc(parameters.zSize, sizeof(uint16_t));
  	uint64_t * accumulator = calloc(parameters.zSize, sizeof(uint64_t));
	uint32_t * decompressedSamples = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint32_t));
	// Error Values
	int16_t * abosluteError = calloc(parameters.zSize, sizeof(int16_t));
	int16_t * relativeError = calloc(parameters.zSize, sizeof(int16_t));
	for (int i = 0; i < parameters.zSize; i++) {
		abosluteError[i] = parameters.maximumError;
		relativeError[i] = parameters.relativeError;	
	}

	/*
		Begin Compression
	*/
	hashFlushCodes();
	hashCodeTableValues();

	if(parameters.debugMode) {
		printParameters(&parameters);
	}

	start = walltime();
	if (parameters.compressionMode == COMPRESS) {
		if(parameters.encodeOrder == BSQ) {
			for (uint16_t z = 0; z < parameters.zSize; z++) {
				for (uint16_t y = 0; y < parameters.ySize; y++) {
					for (uint16_t x = 0; x < parameters.xSize; x++) {
						uint32_t tempResidual = predict(sample, x, y, z, &parameters, sampleRep, diffVector, weights, abosluteError, relativeError);
						encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
						//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
					}
				}
			}
			//encodeFinalStage(accumulator, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
			fillBits(&numWrittenBits, &totalWrittenBytes, &parameters, residuals_file);
		} else if (parameters.encodeOrder == BIP) {
			for (uint16_t y = 0; y < parameters.ySize; y++) {
				for (uint16_t x = 0; x < parameters.xSize; x++) {
					for (uint16_t z = 0; z < parameters.zSize; z++) {
						uint32_t tempResidual = predict(sample, x, y, z, &parameters, sampleRep, diffVector, weights , abosluteError, relativeError);
						// Currently only BSQ encoding mode
						encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
						//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
					}
				}
			}
			//encodeFinalStage(accumulator, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
			fillBits(&numWrittenBits, &totalWrittenBytes, &parameters, residuals_file);
		} else if (parameters.encodeOrder == BIL) {
			for (uint16_t y = 0; y < parameters.ySize; y++)	{
				for (uint16_t z = 0; z < parameters.zSize; z++)	{
					for (uint16_t x = 0; x < parameters.xSize; x++)	{
						uint32_t tempResidual = predict(sample, x, y, z, &parameters, sampleRep, diffVector, weights , abosluteError, relativeError);
						encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
						//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
					}
				}
			}
		}
		//encodeFinalStage(accumulator, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
		fillBits(&numWrittenBits, &totalWrittenBytes, &parameters, residuals_file);
	} else {
		if(parameters.encodeOrder == BSQ) {
			for (uint16_t z = 0; z < parameters.zSize; z++) {
				for (uint16_t y = 0; y < parameters.ySize; y++) {
					for (uint16_t x = 0; x < parameters.xSize; x++) {
						uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, residuals_file, &parameters);
						decompressedSamples[offset(x,y,z,&parameters)] = unPredict(tempResidual, decompressedSamples, x, y, z, &parameters, diffVector, weights, abosluteError, relativeError);
						uint32_t decompressedSample = decompressedSamples[offset(x,y,z,&parameters)];
						if(parameters.pixelType == SIGNED) {
							int32_t signedSample =  decompressedSample - parameters.sMid;
							if(parameters.endianess == BE) {
								signedSample = __builtin_bswap16(signedSample);
							}
							fwrite(&signedSample, 2, 1, decompressedFile);
						} else {
							if(parameters.endianess == BE) {
								decompressedSample = __builtin_bswap16(decompressedSample);
							}
							fwrite(&decompressedSample, 2, 1, decompressedFile);
						}
					}
				}
			}
		} else if(parameters.encodeOrder == BIP) {
			for (uint16_t y = 0; y < parameters.ySize; y++) {
				for (uint16_t x = 0; x < parameters.xSize; x++) {
					for (uint16_t z = 0; z < parameters.zSize; z++) {
						uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, residuals_file, &parameters);
						decompressedSamples[offset(x,y,z,&parameters)] = unPredict(tempResidual, decompressedSamples, x, y, z, &parameters, diffVector, weights, abosluteError, relativeError);
						uint32_t decompressedSample = decompressedSamples[offset(x,y,z,&parameters)];
						if(parameters.pixelType == SIGNED) {
							int32_t signedSample =  decompressedSample - parameters.sMid;
							if(parameters.endianess == BE) {
								signedSample = __builtin_bswap32(signedSample);
							}
							fwrite(&signedSample, 4, 1, decompressedFile);
						} else {
							if(parameters.endianess == BE) {
								decompressedSample = __builtin_bswap32(decompressedSample);
							}
							fwrite(&decompressedSample, 4, 1, decompressedFile);
						}
					}
				}
			}
		} else { // BIL
			for (uint16_t y = 0; y < parameters.ySize; y++)	{
				for (uint16_t z = 0; z < parameters.zSize; z++)	{
					for (uint16_t x = 0; x < parameters.xSize; x++)	{
						uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, residuals_file, &parameters);
						decompressedSamples[offset(x,y,z,&parameters)] = unPredict(tempResidual, decompressedSamples, x, y, z, &parameters, diffVector, weights, abosluteError, relativeError);
						uint32_t decompressedSample = decompressedSamples[offset(x,y,z,&parameters)];
						if(parameters.pixelType == SIGNED) {
							int32_t signedSample =  decompressedSample - parameters.sMid;
							if(parameters.endianess == BE) {
								signedSample = __builtin_bswap32(signedSample);
							}
							fwrite(&signedSample, 4, 1, decompressedFile);
						} else {
							if(parameters.endianess == BE) {
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
	printf("Average time per pixel(compute time/total pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(parameters.xSize*parameters.ySize*parameters.zSize));
	printf("Average time per frame(compute time/z pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(parameters.zSize));

	/*
		END OF COMPUTATION/WRITING
	*/

	fclose(residuals_file);
	// Free up stuff
	for (int i = 0; i < parameters.zSize; i++)
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
