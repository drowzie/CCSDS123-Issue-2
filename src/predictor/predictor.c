#include "include/predictor.h"
#include "../cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long long doubleResPredSample = 0;
long long clippedBin = 0;

void predict(unsigned int * inputSample, unsigned long * residuals, int x, int y, int z, struct arguments * parameters, unsigned int * sampleRep, int * localsum, 
int ** diffVector, long ** weights, long sMin, long sMax, long sMid, int maximumError, int sampleDamping, int sampleOffset) {

	if(x == 0 && y == 0) {
		initWeights(weights, z, parameters);
	}
	/* 
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	wideNeighborLocalSum(sampleRep,localsum,x,y,z,parameters);
	BuildDiffVector(sampleRep,localsum,diffVector,x,y,z,parameters);
	/* 
		Step for calculating prediction sample
	*/
	long long highResSample = computeHighResPredSample(localsum, weights, diffVector, sMid, sMin, sMax, x, y, z, parameters);
	long long predictedSample = computePredictedSample(inputSample, &doubleResPredSample, localsum, weights, diffVector, highResSample, sMid, sMin, sMax, x, y, z, parameters);

	/* 
		Quantization/Sample "compression" part
	*/
	long quantizerIndex = quantization(inputSample, predictedSample, maximumError, x, y, z, parameters);
	sampleRep[offset(x,y,z, parameters)] = sampleRepresentation(inputSample, &clippedBin, predictedSample, quantizerIndex, maximumError, highResSample, sampleDamping, sampleOffset, x, y, z, sMin, sMax, parameters);

	/* 
		Update Weights and mapping
	*/
	long long doubleResError = (2 * clippedBin) - doubleResPredSample; 
	updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);
	residuals[offset(x,y,z,parameters)] = computeMappedQuantizerIndex(quantizerIndex, predictedSample, doubleResPredSample, sMin, sMax, maximumError, x, y, z, parameters);

	if(parameters->debugMode != 0) {
		printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
		printVectors(diffVector, parameters);
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
	unsigned int signValue = (doubleResPredSample & 0x1) != 0 ? -1 : 1;
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
	} else if ((signValue*quantizerIndex) <= omega && (signValue*quantizerIndex >= 0)) {
		return abs(quantizerIndex) << 1;
	} else {
		return (abs(quantizerIndex) << 1) - 1;
	}
}


long quantization(unsigned int * sample, long long predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters) {
	long long predictionResidual = sample[offset(x,y,z,parameters)] - predictedSample;
	long long returnValue = 0;
	if (x == 0 && y == 0)
	{
		return predictionResidual;
	} else {
		returnValue = (abs(predictionResidual) + maximumError) / ((maximumError << 1) + 1);
		predictionResidual = sgn(predictionResidual) * returnValue;
		return predictionResidual;
	}
}

long long computePredictedSample(unsigned int * sample, long long * doubleResPredSample, int * localSum, long ** weightVector, int ** diffVector, long long highResPredSample, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters) {
	if(x > 0 || y > 0) {
		(*doubleResPredSample) = highResPredSample >> (parameters->weightResolution+1);
	} else {
		if (parameters->precedingBands == 0 || z == 0) {
			(*doubleResPredSample) = smid << 1;
		} else {
			(*doubleResPredSample) = sample[offset(x,y,z-1,parameters)] << 1;
		}
	}
	return (*doubleResPredSample) >> 1;
}

unsigned int sampleRepresentation(unsigned int * sample, long long * clippedBinCenter, long predictedSample, long quantizedSample, int maximumError, long long highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, long long smin, long long smax, struct arguments * parameters) {
    if( x == 0 && y == 0) {
        return sample[offset(x,y,z,parameters)];
    } else {
        long long doubleResSample = 0;
        (*clippedBinCenter) = clip(predictedSample + (quantizedSample*((maximumError << 1) + 1)), smin, smax);
        doubleResSample = ( 4 * ((0x1 << parameters->theta) - sampleDamping)) * (((*clippedBinCenter) << parameters->weightResolution) - ((sgn(quantizedSample) * maximumError * sampleOffset) << (parameters->weightResolution - parameters->theta)));
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
	predictedSample += (1 << (parameters->weightResolution + 1));

	long lowerbounds = (smin<<(parameters->weightResolution+2));
	long higherbounds = (smax<<(parameters->weightResolution+1) + 1 << (parameters->weightResolution+1));
	predictedSample = clip(predictedSample, lowerbounds, higherbounds);
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
