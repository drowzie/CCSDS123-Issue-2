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
	uint16_t * sample = NULL;
	//Output compressed image 
	uint8_t * compressedImage = malloc(((parameters.dynamicRange + 7)/8)*parameters.xSize*parameters.ySize*parameters.zSize);
	//Files to write 
	FILE * residuals_file;
	FILE * decompressedFile;
	
	
	if(parameters.compressionMode == COMPRESS) {
		sample = malloc(parameters.xSize*parameters.ySize*parameters.zSize*sizeof(uint16_t));
		residuals_file = fopen("Encoded.raw", "w+b");
		readIntSamples(&parameters, parameters.fileName, sample);
	} else {
		decompressedFile = fopen("DecompressedFile.raw", "wb");
		residuals_file = fopen(parameters.fileName, "rb");
	}

	/* --------------------
		COMPRESSION
	-----------------------*/
	unsigned int totalWrittenBytes = 0;
	printf("Computing: \n Encode order: %d Offset order: %d \n", parameters.encodeOrder, parameters.imageOrder);
	start = walltime();

	if(parameters.compressionMode == COMPRESS) {
		int error = compressImage(sample, compressedImage, &totalWrittenBytes, &parameters);
		if(error) {
			printf("Compression did not complete \n");
			exit(error);
		}
		fwrite(compressedImage, 1, totalWrittenBytes, residuals_file);
	} else {
		int error = decompressImage(residuals_file, decompressedFile, &parameters);
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
	return 0;
}
