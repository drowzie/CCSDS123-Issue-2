#include "include/predictor.h"
#include "../cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void predict(unsigned int * inputSample, unsigned long * residuals, int x, int y, int z, struct arguments * parameters, unsigned int * sampleRep, int * localsum, 
int ** diffVector, long ** weights, long sMin, long sMax, long sMid, int maximumError, int sampleDamping, int sampleOffset) {
	/* 
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	wideNeighborLocalSum(sampleRep,localsum,x,y,z,parameters);
	BuildDiffVector(sampleRep,localsum,diffVector,x,y,z,parameters);
	/* 
		Step for calculating prediction sample
	*/
	long long highResSample = computeHighResPredSample(localsum, weights, diffVector, sMid, sMin, sMax, x, y, z, parameters);
	long long doubleResPredSample = 0;
	long long predictedSample = computePredictedSample(inputSample, &doubleResPredSample, localsum, weights, diffVector, highResSample, sMid, sMin, sMax, x, y, z, parameters);
	int delta = inputSample[offset(x,y,z,parameters)] - predictedSample;
	/* 
		Quantization/Sample "compression" part
	*/
	long quantizerIndex = quantization(inputSample, predictedSample, maximumError, x, y, z, parameters);
	long clippedBin = clippedBinCenter(predictedSample, quantizerIndex, maximumError, sMin, sMax);
	sampleRep[offset(x,y,z, parameters)] = sampleRepresentation(inputSample, clippedBin, predictedSample, quantizerIndex, maximumError, highResSample, sampleDamping, sampleOffset, x, y, z, parameters);
	if(x == 0 && y == 0) {
		initWeights(weights, z, parameters);
	} else {
		long long doubleResError = (2 * clippedBin) - doubleResPredSample; 
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);
	}
	residuals[offset(x,y,z,parameters)] = computeMappedQuantizerIndex(quantizerIndex, predictedSample, doubleResPredSample, sMin, sMax, maximumError, x, y, z, parameters);
	/* 
		Update Weights and mapping
	*/

	if(parameters->debugMode != 0) {
		printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
		printVectors(diffVector, parameters);
		printVectors(weights, parameters);
		printf("High resolution Sample is %lld \n", highResSample);
		printf("predicted value is %lld \n", predictedSample);
		printf("quantizer to sample is %ld \n", quantizerIndex);
		printf("input sample is %u \n", inputSample[offset(x,y,z, parameters)]);
		printf("sample rep is %u \n", sampleRep[offset(x,y,z, parameters)]);
		printf("Mapped residual: %lu \n", residuals[offset(x,y,z,parameters)]);
		printf("---------------------------\n");
	}
}


/* 
	CCSDS 123 Issue 2 Chapter 4.11
 */

long computeMappedQuantizerIndex(long quantizerIndex, long long predictedSample, long long doubleResPredSample, long smin, long smax, int maximumError, int x, int y, int z, struct arguments * parameters) {
	unsigned long long omega = 0;
	//unsigned int signValue = ((int)doubleResPredSample & 0x1) != 0 ? -1 : 1;
	long long temp1 = predictedSample - smin;
	long long temp2 = smax - predictedSample;
	
	if (x == 0 && y == 0) {
		omega = temp1 > temp2 ? temp2 : temp1;
	} else {
		temp1 = ((temp1 + maximumError) / ((maximumError << 1) + 1));
		temp2 = ((temp2 + maximumError) / ((maximumError << 1) + 1));
		omega = temp1 > temp2 ? temp2 : temp1;
	}
	if (abs(quantizerIndex) > omega) {
		return abs(quantizerIndex) + omega;
	} else if (doubleResPredSample % 2 == 0 && quantizerIndex >= 0 || doubleResPredSample % 2 != 0 && quantizerIndex <= 0) {
		return abs(quantizerIndex) *2;
	} else {
		return (abs(quantizerIndex) *2) - 1;
	}
}


long quantization(unsigned int * sample, long long predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters) {
	long long predictionResidual = sample[offset(x,y,z,parameters)] - predictedSample;
	long long returnValue = 0;
	if (x == 0 && y == 0) {
		return predictionResidual;
	} else {
		return sgn(predictionResidual) * ((abs(predictionResidual) + maximumError) / ((maximumError << 1) + 1));
	}
}

long long computePredictedSample(unsigned int * sample, long long * doubleResPredSample, int * localSum, long ** weightVector, int ** diffVector, long long highResPredSample, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters) {
	if(x > 0 || y > 0) {
		(*doubleResPredSample) = highResPredSample >> (parameters->weightResolution+1);
	} else {
		if (parameters->precedingBands == 0 || z == 0) {
			(*doubleResPredSample) = 2*smid;
		} else {
			(*doubleResPredSample) = 2*sample[offset(x,y,z-1,parameters)];
		}
	}
	return (*doubleResPredSample) >> 1;
}

long clippedBinCenter(long predictedSample, long quantizedSample, int maximumError, long sMin, long sMax) {
	return clip(predictedSample + (quantizedSample*((maximumError << 1) + 1)), sMin, sMax);
}

unsigned int sampleRepresentation(unsigned int * sample, long clippedBinCenter, long predictedSample, long quantizedSample, int maximumError, long long highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, struct arguments * parameters) {
    if( x == 0 && y == 0) {
        return sample[offset(x,y,z,parameters)];
    } else {
        long long doubleResSample = 0;
        doubleResSample = ( 4 * ((0x1 << parameters->theta) - sampleDamping)) * ((clippedBinCenter << parameters->weightResolution) - ((sgn(quantizedSample) * maximumError * sampleOffset) << (parameters->weightResolution - parameters->theta)));
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
	predictedSample = modR(diffPredicted + ((localSum[offset(x,y,z, parameters)] - (smid *4)) << parameters->weightResolution), parameters->registerSize);
	predictedSample += (smid << (parameters->weightResolution + 2));
	predictedSample += (1 << (parameters->weightResolution + 1));

	long lowerbounds = (smin<<(parameters->weightResolution+2));
	long higherbounds = (smax<<(parameters->weightResolution+2) + (1 << (parameters->weightResolution+1)));
	return clip(predictedSample, lowerbounds, higherbounds);
}

long long innerProduct(long ** weightVector, int ** diffVector, int z, struct arguments * parameters) {
	long long diffPredicted = 0;
	int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
	if (z > 0) {
		for(int i = 0; i < currentPredBands; i++) {
			diffPredicted += diffVector[CENTRAL][i] * weightVector[CENTRAL][i];
		}
	}

	if (parameters->mode == FULL) {
		for (int i = 1; i < 4; i++) {
			diffPredicted += diffVector[i][0] * weightVector[i][0];
		}
	}
	return diffPredicted;
}
