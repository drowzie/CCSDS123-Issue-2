#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "cliparser/cli.h"
#include "compressor.h"
#include "utils/utilities.h"
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
	//Output compressed image 
	uint8_t * compressedImage = malloc(((parameters.dynamicRange + 7)/8)*parameters.xSize*parameters.ySize*parameters.zSize);
	//Files to write 
	FILE * residuals_file = fopen("Encoded.bin", "w+b");
	readIntSamples(&parameters, "../issue2/HICO_L1B_1.BSQ", sample);
	/* --------------------
		COMPRESSION
	-----------------------*/
	unsigned int totalWrittenBytes = 0;
	printf("Computing: \n Encode order: %d Offset order: %d \n", parameters.encodeOrder, parameters.imageOrder);
	start = walltime();
	int error = compressImage(sample, compressedImage, &totalWrittenBytes, &parameters);
	if(error) {
		printf("Compression done fucked \n");
		exit(error);
	}
	computeTime += walltime() - start;
	/* 
		END OF COMPRESSION
	*/

	fwrite(compressedImage, 1, totalWrittenBytes, residuals_file);

	printf("\n");
	printf("Compute time:          %4.6f s\n",computeTime);
	printf("Average time per pixel(compute time/total pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(parameters.xSize*parameters.ySize*parameters.zSize));
	printf("Average time per frame(compute time/z pixels):          %7.4f ms\n",(computeTime*1e3)/(double)(parameters.zSize));

	/*
		END OF COMPUTATION/WRITING
	*/
	fclose(residuals_file);
	return 0;
}
