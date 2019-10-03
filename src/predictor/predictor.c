#include "include/predictor.h"
#include "localdiffrences/include/localdiffrences.h"
#include "utils/include/utilities.h"
#include "localsum/include/localsum.h"
#include "weights/include/weights.h"

#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

void predict(struct arguments * parameters, int * inputSample, unsigned short int * residuals) {

	// Init Stuff
	int * localsum = (int*) calloc(parameters->xSize*parameters->ySize*parameters->zSize, sizeof(int));
	if(localsum == NULL) {
        fprintf(stderr, "Error in allocating the localsum\n");
        exit(EXIT_FAILURE);
    }

	int ** weights = (int **) calloc((parameters->mode != REDUCED ? 4 : 1), sizeof(int *));
    for (int i=0; i<(parameters->mode != REDUCED ? 4 : 1); i++) { 
         weights[i] = (int *)calloc((i == 4 ? parameters->precedingBands : 1), sizeof(int)); 
    }

	int ** diffVector = (int **) calloc((parameters->mode != REDUCED ? 4 : 1), sizeof(int *));
    for (int i=0; i<(parameters->mode != REDUCED ? 4 : 1); i++) { 
         diffVector[i] = (int *)calloc((i == 4 ? parameters->precedingBands : 1), sizeof(int)); 
    }

    if(weights == NULL){
        fprintf(stderr, "Error in allocating the weights\n");
        exit(EXIT_FAILURE);
    }

	// Compute predictions
	for (int z = 0; z < parameters->zSize; z++)
	{
		for (int y = 0; y < parameters->ySize; y++)
		{
			for (int x = 0; x < parameters->xSize; x++)
			{
				printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
				wideNeighborLocalSum(inputSample,localsum,x,y,z,parameters);
				BuildDiffVector(inputSample,localsum,diffVector,x,y,z,parameters);
				printVectors(diffVector, parameters);
			}
		}
	}
	printArray(inputSample, parameters);
	printArray(localsum, parameters);
	printf("\nLOCAL SUM ARRAY\n\n");

	// Free up stuff
	free(localsum);
}


void quantization(int * sample, int * predictedSample, int * fidelityControl, int * predictionResidual, int * quantizedSample, struct arguments * parameters) {
	for (int z = 0; z < parameters->zSize; z++) {
		for (int y = 0; y < parameters->ySize; y++) {
			for (int x = 0; x < parameters->xSize; x++) {
				if((x == 0 && y == 0)) {
					quantizedSample[offset(x,y,z, parameters)] = predictionResidual[offset(x,y,z, parameters)]; // prediction at 0
				} else {

				}
			}
		}
	}	
}