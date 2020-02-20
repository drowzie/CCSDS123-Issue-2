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
	double computeTime=0, predictionTime = 0, encoderTime = 0;
	double readTime=0;
  	double writeTime=0;

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
	//Predictor
	int32_t * weights = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(int32_t ));
	int32_t * diffVector = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(int32_t ));
	// Encoder
    uint16_t * counter = malloc(sizeof(uint16_t)*parameters.zSize);
    uint64_t * accumulator = malloc(sizeof(uint64_t)*parameters.zSize);

	/* 
		Mallocs for decompression
	*/
	uint32_t * decompressedSamples = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint32_t));
	uint32_t * residuals = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint32_t));
	int32_t * decompressionWeights = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(int32_t ));
	int32_t * decompressionDiffVector = malloc((parameters.mode != REDUCED ? parameters.precedingBands+3 : parameters.precedingBands) * sizeof(int32_t ));

	FILE * residuals_file = fopen("Encoded.bin", "w+b");
	FILE * deltafile = fopen("Encoded.bin.delta", "w+b");

	unsigned int numWrittenBits = 0;
	unsigned int totalWrittenBytes = 0;
	readIntSamples(&parameters, parameters.fileName, sample);
	/* 
		Hybrid Encoder Related Init Functions
	*/
	//hashFlushCodes();
	//hashCodeTableValues();

	printf("Computing \n");
	start = walltime();
	printf("Encode order: %d Offset order: %d \n", parameters.encodeOrder, parameters.imageOrder);
	if(parameters.encodeOrder == BSQ) {
		//#pragma omp parallel for num_threads(16)
		for (uint16_t z = 0; z < parameters.zSize; z++) {
			for (uint16_t y = 0; y < parameters.ySize; y++) {
				for (uint16_t x = 0; x < parameters.xSize; x++) {
					residuals[offset(x,y,z,&parameters)] = predict(sample, x, y, z, &parameters, sampleRep, diffVector, weights, 
					0, 0, 0, 0, 0);
					//fwrite((&tempRes), 4, 1, deltafile);

					decompressedSamples[offset(x,y,z,&parameters)] = unPredict(residuals, decompressedSamples, x, y, z, &parameters, decompressionDiffVector, decompressionWeights,
					0, 0, 0, 0, 0);

					if(sample[offset(x,y,z,&parameters)] != decompressedSamples[offset(x,y,z,&parameters)]) {
						printf("x:%d, y %d \n",x,y);
						printf("Sample %d vs unpredicted %d \n", sample[offset(x,y,z,&parameters)], decompressedSamples[offset(x,y,z,&parameters)]);
						
					} 
					//Currently only BSQ encoding mode
					//encodeSampleAdaptive(residuals[offset(x,y,z,&parameters)], counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
					//encodeHybrid(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
				}
			}
			//encodeFinalStage(accumulator, z,  &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
		}
	} 
	else if (parameters.encodeOrder == BIP) {
		for (uint16_t y = 0; y < parameters.ySize; y++) {
			for (uint16_t x = 0; x < parameters.xSize; x++) {
				for (uint16_t z = 0; z < parameters.zSize; z++) {
					uint32_t tempResidual = predict(sample, x, y, z, &parameters, sampleRep, diffVector, weights, 0, 0, 0, 0, 0);
					// Currently only BSQ encoding mode
					fwrite((&tempResidual), 4, 1, deltafile);
					encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
				}
			}
		}
	} else if (parameters.encodeOrder == BIL) {
		for (uint16_t y = 0; y < parameters.ySize; y++)	{
			for (uint16_t z = 0; z < parameters.zSize; z++)	{
				for (uint16_t x = 0; x < parameters.xSize; x++)	{
					uint32_t tempResidual = predict(sample, x, y, z, &parameters, sampleRep, diffVector, weights, 0, 0, 0, 0, 0);
					// Currently only BSQ encoding mode
					fwrite((&tempResidual), 4, 1, deltafile);
					encodeSampleAdaptive(tempResidual, counter, accumulator, x, y, z, &totalWrittenBytes, &numWrittenBits, residuals_file, &parameters);
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
	fclose(deltafile);

	// Free up stuff
	free(accumulator);
	free(counter);
	free(sampleRep);
	free(sample);
 	free(weights);
	free(diffVector);

	return 0;
}
