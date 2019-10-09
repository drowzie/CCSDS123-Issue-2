#include "include/predictor.h"
#include "localdiffrences/include/localdiffrences.h"
#include "utils/include/utilities.h"
#include "localsum/include/localsum.h"
#include "weights/include/weights.h"

#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void predict(struct arguments * parameters, int * inputSample, unsigned short int * residuals) {
	unsigned int sMin = 0;
    unsigned int sMax = (0x1 << parameters->dynamicRange) - 1;
    unsigned int sMid = 0x1 << (parameters->dynamicRange - 1);
	int maxmimumError = 0; // Zero means lossless
	
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

int quantization(int * sample, int predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters) {
	int predictionResidual = sample[offset(x,y,z,parameters)] - predictedSample;
	
	if (x == 0 && y == 0)
	{
		return predictionResidual;
	} else {
		return sgn(predictionResidual) * floor((predictionResidual+maximumError)/((maximumError << 1) + 1));
	}
	

}


int computePredictedSample(int * sample, int * localSum, int ** weightVector, int ** diffVector, long long highResPredSample, int smid, int smin, int smax, int x, int y, int z, struct arguments * parameters) {
	long long predictedSample = highResPredSample;
	if(x > 0 || y > 0) {
		if (z > 0) {
			predictedSample = predictedSample >> (parameters->weightResolution+1);
		}
	} else if (x == 0 && y == 0 && parameters->precedingBands > 0 && z > 0) {
		predictedSample = 2 * sample[offset(x,y,z-1,parameters)];
	} else if (x == 0 && y == 0 && (parameters->precedingBands == 0 || z == 0)) {
		predictedSample = 2 * smid;
	} else {
		printf("computePredicted Sample should not arrive here\n");
		exit(EXIT_FAILURE);
	}
	return (int)predictedSample >> 1;
}

int sampleRepresentation(int * sample, int predictedSample, int quantizedSample, int maximumError, int * highResPredSample, int * sampleDamping, int * sampleOffset, int x, int y, int z, int smin, int smax, struct arguments * parameters) {
    if( x == 0 & y == 0) {
        return sample[offset(x,y,z,parameters)];
    } else {
        long long doubleResSample = 0;
        int clippedBinCenter = clip(predictedSample + (quantizedSample*((maximumError << 1) + 1)), smin, smax);
        clippedBinCenter = clippedBinCenter << parameters->weightResolution;
        doubleResSample = (4 * ((1 << parameters->theta) - sampleDamping[z])) * (clippedBinCenter - ((sgn(quantizedSample) * maximumError * sampleOffset[z]) << (parameters->weightResolution - parameters->theta)));
        doubleResSample += (sampleDamping[z] * predictedSample - (sampleDamping[z] << parameters->weightResolution));
        doubleResSample = doubleResSample >> (parameters->weightResolution + parameters->theta + 1);
        return (int)doubleResSample;
    }
}

long long computeHighResPredSample(int * localSum, int ** weightVector, int ** diffVector, int smid, int smin, int smax, int x, int y, int z, struct arguments * parameters) {
	long long diffPredicted = 0;
	long long predictedSample = 0;
	diffPredicted = innerProduct(weightVector, diffVector, z, parameters);
	predictedSample = modR(diffPredicted + ((localSum[offset(x,y,z, parameters)] - 4 * smid) << parameters->weightResolution), parameters->dynamicRange);
	predictedSample += (smid << (parameters->weightResolution + 2));
	predictedSample += (0x1 << (parameters->weightResolution + 1));
	predictedSample = clip(predictedSample, (smin << (parameters->weightResolution+2)), (smax << (parameters->weightResolution+2)));
	return predictedSample;
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
		for (int i = 0; i < 3; i++) {
			diffPredicted += diffVector[i][0] * weightVector[i][0];
		}
	}
	return diffPredicted;
}