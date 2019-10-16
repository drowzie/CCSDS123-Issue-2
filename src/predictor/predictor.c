#include "include/predictor.h"
#include "localdiffrences/include/localdiffrences.h"
#include "utils/include/utilities.h"
#include "localsum/include/localsum.h"
#include "weights/include/weights.h"

#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void predict(struct arguments * parameters, unsigned int * inputSample, unsigned int * residuals) {
	long  sMin = 0;
    long  sMax = (0x1 << parameters->dynamicRange) - 1;
    long  sMid = 0x1 << (parameters->dynamicRange - 1);
	printf("sMax %ld,SMin %ld,smid %ld", sMax, sMin, sMid);
	int maxmimumError = 0; // Zero means lossless
	int sampleDamping = 0;
	int sampleOffset = 0;
	
	// Init Stuff
	int * localsum = (int*) calloc(parameters->xSize*parameters->ySize*parameters->zSize, sizeof(int));
	if(localsum == NULL) {
        fprintf(stderr, "Error in allocating the localsum\n");
        exit(EXIT_FAILURE);
    }
	
	unsigned int * sampleRep = (int*) malloc(parameters->xSize*parameters->ySize*parameters->zSize*sizeof(unsigned int));

	long ** weights = (long **) malloc((parameters->mode != REDUCED ? 4 : 1) * sizeof(long *));
    for (int i=0; i<(parameters->mode != REDUCED ? 4 : 1); i++) {
         weights[i] = (long *)calloc((i == 4 ? parameters->precedingBands : 1), sizeof(long)); 
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
	long long doubleResPredSample = 0;
	long long clippedBin = 0;
	for (int z = 0; z < parameters->zSize; z++)
	{
		for (int y = 0; y < parameters->ySize; y++)
		{
			for (int x = 0; x < parameters->xSize; x++)
			{

				if(x == 0 && y == 0) {
					initWeights(weights, z, parameters);
				}

				printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
				wideNeighborLocalSum(sampleRep,localsum,x,y,z,parameters);
				BuildDiffVector(sampleRep,localsum,diffVector,x,y,z,parameters);

				long long highResSample = computeHighResPredSample(localsum, weights, diffVector, sMid, sMin, sMax, x, y, z, parameters);
				printf("High resolution Sample is %lld \n", highResSample);

				long predictedSample = computePredictedSample(inputSample, &doubleResPredSample, localsum, weights, diffVector, highResSample, sMid, sMin, sMax, x, y, z, parameters);
				printf("predicted value is %ld \n", predictedSample);

				long quantizerIndex = quantization(inputSample, predictedSample, maxmimumError, x, y, z, parameters);
				printf("quantizer index is %ld \n", quantizerIndex);

				sampleRep[offset(x,y,z, parameters)] = sampleRepresentation(inputSample, &clippedBin, predictedSample, quantizerIndex, maxmimumError, highResSample, sampleDamping, sampleOffset, x, y, z, sMin, sMax, parameters);
				printf("input sample is %u \n", inputSample[offset(x,y,z, parameters)]);
				printf("sample rep is %u \n", sampleRep[offset(x,y,z, parameters)]);

				long long doubleResError = (2 * clippedBin) - doubleResPredSample; 
				updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);
				residuals[offset(x,y,z,parameters)] = computeMappedQuantizerIndex(quantizerIndex, predictedSample, doubleResPredSample, sMin, sMax, maxmimumError, x, y, z, parameters);
				printf("Mapped residual: %u \n", residuals[offset(x,y,z,parameters)]);
				printf("---------------------------\n");

			}
		}
	}
	printf("Samples \n");
	printArray(inputSample, parameters);
	printf("Sample representation \n");
	printArray(sampleRep, parameters);
	printf("Mapped residuals \n");
	printArray(residuals,parameters);

	// Free up stuff
	free(weights);
	free(diffVector);
	free(localsum);
}


/* 
	CCSDS 123 Issue 2 Chapter 4.11
 */

long computeMappedQuantizerIndex(long quantizerIndex, long long predictedSample, long long doubleResPredSample, long smin, long smax, int maximumError, int x, int y, int z, struct arguments * parameters) {
	long long omega = 0;
	int signValue = (doubleResPredSample & 0x1) != 0 ? -1 : 1;
	if (x == 0 && y == 0) {
		omega = (predictedSample - smin) > (smax - predictedSample) ? (smax - predictedSample) : (predictedSample - smin);
	} else {
		long long temp1 = (((predictedSample - smin) + maximumError) / (2 * maximumError + 1));
		long long temp2 = (((smax - predictedSample ) + maximumError) / (2 * maximumError + 1));
		omega = temp1 > temp2 ? temp2 : temp1;
	}

	if (abs(quantizerIndex) > omega) {
		return abs(quantizerIndex) + omega;
	} else if ((signValue*quantizerIndex) <= omega && signValue*quantizerIndex >= 0) {
		return 2 * abs(quantizerIndex);
	} else {
		return 2 * abs(quantizerIndex) - 1;
	}
}

long quantization(unsigned int * sample, long long predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters) {
	long long predictionResidual = sample[offset(x,y,z,parameters)] - predictedSample;
	if (x == 0 && y == 0)
	{
		return predictionResidual;
	} else {
		return sgn(predictionResidual) * ((predictionResidual+maximumError)/((maximumError * 1) + 1));
	}
}

long computePredictedSample(unsigned int * sample, long long * doubleResPredSample, int * localSum, long ** weightVector, int ** diffVector, long long highResPredSample, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters) {
	*doubleResPredSample = highResPredSample;
	if(x > 0 || y > 0) {
		*doubleResPredSample = (*doubleResPredSample) >> (parameters->weightResolution+1);
	} else {
		if (parameters->precedingBands == 0 || z == 0) {
			*doubleResPredSample = 2 * smid;
		} else {
			*doubleResPredSample = 2 * sample[offset(x,y,z-1,parameters)];
		}
	}
	return (long) (*doubleResPredSample) >> 1;
}

unsigned int sampleRepresentation(unsigned int * sample, long long * clippedBinCenter, long predictedSample, long quantizedSample, int maximumError, long long highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, long long smin, long long smax, struct arguments * parameters) {
    if( x == 0 && y == 0) {
        return sample[offset(x,y,z,parameters)];
    } else {
        long long doubleResSample = 0;
        *clippedBinCenter = clip(predictedSample + (quantizedSample*((maximumError * 2) + 1)), smin, smax);
        doubleResSample = ( 4 * ((0x1 << parameters->theta) - sampleDamping)) * ((*clippedBinCenter << parameters->weightResolution) - ((sgn(quantizedSample) * maximumError * sampleOffset) << (parameters->weightResolution - parameters->theta)));
        doubleResSample += ((sampleDamping * highResPredSample) - (sampleDamping << (parameters->weightResolution + 1)));
        doubleResSample = doubleResSample >> (parameters->weightResolution + parameters->theta + 1);
		doubleResSample = (doubleResSample + 1) >> 1;
        return (unsigned int)doubleResSample;
    }
}

long long computeHighResPredSample(unsigned int * localSum, long ** weightVector, int ** diffVector, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters) {
	long long diffPredicted = 0;
	long long predictedSample = 0;
	diffPredicted = innerProduct(weightVector, diffVector, z, parameters);
	predictedSample = modR(diffPredicted + ((localSum[offset(x,y,z, parameters)] - (smid << 2)) << parameters->weightResolution), parameters->registerSize);
	predictedSample += (smid << (parameters->weightResolution + 2));
	predictedSample += (0x1 << (parameters->weightResolution + 1));
	predictedSample = clip(predictedSample, (smin << (parameters->weightResolution+2)), ((smax << (parameters->weightResolution+2)) + (0x1 << (parameters->weightResolution+1))));
	return predictedSample;
}

long long innerProduct(long ** weightVector, int ** diffVector, int z, struct arguments * parameters) {
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