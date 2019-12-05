#include "include/predictor.h"
#include "../cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


uint32_t predict(uint32_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, uint32_t * sampleRep, int32_t * localsum, 
int32_t * diffVector, int32_t * weights, int32_t sMin, int32_t sMax, int32_t sMid, uint32_t maximumError, uint32_t sampleDamping, uint32_t sampleOffset, uint32_t interbandOffset, int32_t intrabandExponent) {
	/* 
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	int64_t highResSample = 0;
	if(x+y != 0) {
		narrowNeighborLocalSum(sampleRep,localsum, sMid, x,y,z,parameters);
		BuildDiffVector(sampleRep,localsum,diffVector,x,y,z,parameters);
		highResSample = computeHighResPredSample(localsum, weights, diffVector, sMid, sMin, sMax, x, y, z, parameters);
	}
	/* 
		Step for calculating prediction sample and doubleResPredSample
	*/
	int64_t doubleResPredSample = 0; // Calculated inside function computePredictedSample
	int64_t predictedSample = computePredictedSample(inputSample, &doubleResPredSample, localsum, highResSample, sMid, sMin, sMax, x, y, z, parameters);
	/* 
		Quantization/Sample "compression" part
	*/
	int32_t quantizerIndex = quantization(inputSample, predictedSample, maximumError, x, y, z, parameters);
	int32_t clippedBin = clippedBinCenter(predictedSample, quantizerIndex, maximumError, sMin, sMax);
	sampleRep[offset(x,y,z, parameters)] = sampleRepresentation(inputSample, clippedBin, predictedSample, quantizerIndex, maximumError, highResSample, sampleDamping, sampleOffset, x, y, z, parameters);
	if(x+y == 0) {
		initWeights(weights, z, parameters);
	} else {
		int64_t doubleResError = (clippedBin << 1) - doubleResPredSample;
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, interbandOffset, intrabandExponent, parameters);
	}
	int32_t residual = computeMappedQuantizerIndex(quantizerIndex, predictedSample, doubleResPredSample, sMin, sMax, maximumError, x, y, z, parameters);

	if(parameters->debugMode != 0) {
		printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
		printf("High resolution Sample is %ld \n", highResSample);
		printf("predicted value is %ld \n", predictedSample);
		printf("quantizer to sample is %d \n", quantizerIndex);
		printf("input sample is %u \n", inputSample[offset(x,y,z, parameters)]);
		printf("sample rep is %u \n", sampleRep[offset(x,y,z, parameters)]);
		printf("Mapped residual: %u \n", residual);
		printf("---------------------------\n");
	}
	return residual;
}


/* 
	CCSDS 123 Issue 2 Chapter 4.11
 */

int32_t computeMappedQuantizerIndex(int32_t quantizerIndex, int64_t predictedSample, int64_t doubleResPredSample, int32_t smin, int32_t smax, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {	
	int64_t omega = 0;
	//unsigned int signValue = ((int)doubleResPredSample & 0x1) != 0 ? -1 : 1;
	int64_t temp1 = predictedSample - smin;
	int64_t temp2 = smax - predictedSample;
	
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
		return abs(quantizerIndex) << 1;
	} else {
		return (abs(quantizerIndex) << 1) - 1;
	}
}


int32_t quantization(uint32_t * sample, int64_t predictedSample, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
	int64_t predictionResidual = sample[offset(x,y,z,parameters)] - predictedSample;
	int64_t returnValue = 0;
	if (x == 0 && y == 0) {
		return predictionResidual;
	} else {
		return sgn(predictionResidual) * ((abs(predictionResidual) + maximumError) / ((maximumError << 1) + 1));
	}
}

int64_t computePredictedSample(uint32_t * sample, int64_t * doubleResPredSample, int32_t * localsum, int64_t highResPredSample, int32_t smid, int32_t smin, int32_t smax, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
	if(x+y == 0) {
		if(z == 0 || parameters->precedingBands == 0) {
			(*doubleResPredSample) = smid << 1;
		} else {
			(*doubleResPredSample) = sample[offset(x,y,z-1,parameters)] << 1;
		}
	} else {
		(*doubleResPredSample) = highResPredSample >> (parameters->weightResolution+1);
	}
	return (*doubleResPredSample) >> 1;
}

int32_t clippedBinCenter(int32_t predictedSample, int32_t quantizedSample, uint32_t maximumError, int32_t sMin, int32_t sMax) {
	return clip(predictedSample + (quantizedSample*((maximumError << 1) + 1)), sMin, sMax);
}

uint32_t sampleRepresentation(uint32_t * sample, int32_t clippedBinCenter, int32_t predictedSample, int32_t quantizedSample, uint32_t maximumError, int64_t highResPredSample, int sampleDamping, int sampleOffset, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
    if( x == 0 && y == 0) {
        return sample[offset(x,y,z,parameters)];
    } else {
        int64_t doubleResSample = 0;
        doubleResSample = ( 4 * ((0x1 << parameters->theta) - sampleDamping)) * ((clippedBinCenter << parameters->weightResolution) - ((sgn(quantizedSample) * maximumError * sampleOffset) << (parameters->weightResolution - parameters->theta)));
        doubleResSample += ((sampleDamping * highResPredSample) - (sampleDamping << (parameters->weightResolution + 1)));
        doubleResSample = doubleResSample >> (parameters->weightResolution + parameters->theta + 1);
		doubleResSample = (doubleResSample + 1) >> 1;
        return (uint32_t)doubleResSample;
    }
}

int64_t computeHighResPredSample(int32_t * localsum, int32_t * weightVector, int32_t * diffVector, int32_t smid, int32_t smin, int32_t smax, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
	int32_t diffPredicted = 0;
	int64_t predictedSample = 0;
	diffPredicted = innerProduct(weightVector, diffVector, z, parameters);
	///////
	int32_t tmpValue = localsum[offset(x,y,z, parameters)] - (smid << 2);
	int sgn = tmpValue < 0;
	tmpValue = abs(tmpValue) << parameters->weightResolution;
	tmpValue = sgn ? -1 * tmpValue : tmpValue;
	predictedSample = modR(diffPredicted + tmpValue, parameters->registerSize);
	////
	predictedSample += (smid << (parameters->weightResolution + 2));
	predictedSample += (1 << (parameters->weightResolution + 1));

	int32_t lowerbounds = (smin<<(parameters->weightResolution+2));
	int32_t higherbounds = (smax<<(parameters->weightResolution+2) + (1 << (parameters->weightResolution+1)));
	return clip(predictedSample, lowerbounds, higherbounds);
}

int32_t innerProduct(int32_t * weightVector, int32_t * diffVector, uint16_t z, struct arguments * parameters) {
	int32_t diffPredicted = 0;
	int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;
	for(int i = 0; i < currentPredictionBand; i++) {
		diffPredicted += diffVector[i] * weightVector[i];
	}
	if(parameters->mode == FULL) {
		for (int i = 0; i < 3; i++)
		{
			diffPredicted += diffVector[parameters->precedingBands+i] * weightVector[parameters->precedingBands+i];
		}
	}
	return diffPredicted;
}
