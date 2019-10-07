#include "include/predictor.h"
#include "localdiffrences/include/localdiffrences.h"
#include "utils/include/utilities.h"
#include "localsum/include/localsum.h"
#include "weights/include/weights.h"

#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

void predict(struct arguments * parameters, int * inputSample, unsigned short int * residuals) {
	unsigned int sMin = 0;
    unsigned int sMax = (0x1 << parameters->dynamicRange) - 1;
    unsigned int sMid = 0x1 << (parameters->dynamicRange - 1);
	
	// Init Stuff
	int * localsum = (int*) calloc(parameters->xSize*parameters->ySize*parameters->zSize, sizeof(int));
	if(localsum == NULL) {
        fprintf(stderr, "Error in allocating the localsum\n");
        exit(EXIT_FAILURE);
    }

	int ** weights = (int **) malloc((parameters->mode != REDUCED ? 4 : 1) * sizeof(int *));
    for (int i=0; i<(parameters->mode != REDUCED ? 4 : 1); i++) { 
         weights[i] = (int *)calloc((i == 4 ? parameters->precedingBands : 1), sizeof(int)); 
    }

	int ** diffVector = (int **) malloc((parameters->mode != REDUCED ? 4 : 1) * sizeof(int *));
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
				initWeights(weights, z, parameters);

				printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
				wideNeighborLocalSum(inputSample,localsum,x,y,z,parameters);
				BuildDiffVector(inputSample,localsum,diffVector,x,y,z,parameters);
				printVectors(diffVector, parameters);
			}
		}
	}
	printArray(inputSample, parameters);
	printArray(localsum, parameters);

	// Free up stuff
	free(weights);
	free(diffVector);
	free(localsum);
}



int computePredictedSample(int * sample, int ** weightVector, int ** diffector, int smid, int smax, int * predictedSample, int x, int y, int z, struct arguments * parameters) {
    long long diffPredicted = 0;
	if(x > 0 || y > 0) {
		if (z > 0) {
			
		}
	} else if (x == 0 && y == 0 && parameters->precedingBands > 0 && z > 0) {

	} else if (x == 0 && y == 0 && (parameters->precedingBands == 0 || z == 0)) {

	} else {
		printf("computePredicted Sample should not arrive here\n");
		exit(EXIT_FAILURE);
	}
}

long long innerProduct(int ** weightVector, int ** diffVector, int z, struct arguments * parameters) {
	long long diffPredicted = 0;
	int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
	if (z > 0) {
		for(int i = 0; i < currentPredBands; i++) {
			diffPredicted += diffVector[0][i] * weightVector[0][i];
		}
	}

	if (parameters->mode == FULL) {
	for (int i = 0; i < 3; i++)
		{
			diffPredicted += diffVector[i][0] * weightVector[i][0];
		}
	}
	return diffPredicted;
}