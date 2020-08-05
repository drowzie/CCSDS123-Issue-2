#include "predictor.h"
#include "cliparser/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

uint16_t predict(uint16_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int64_t * diffVector, int32_t ** weights) {
	/*
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	int32_t(*sumFunc)(uint16_t *, uint16_t, uint16_t, uint16_t, struct arguments *);
	switch (parameters->sumType) {
	case NEIGHBOR:
		sumFunc = &wideNeighborLocalSum;
		break;
	case COLUMN:
		sumFunc = &wideColumnLocalSum;
		break;
	default:
		sumFunc = NULL;
		break;
	}
	if(x+y != 0) {

		BuildDiffVector(inputSample, diffVector, x, y, z, parameters, sumFunc);
	}
	/* 
		Step for calculating prediction sample and scaledPredicted
	*/
	uint32_t scaledPredicted = computeScaledPredicted(inputSample, weights, diffVector, (*sumFunc)(inputSample, x, y, z, parameters), x, y, z, parameters);
	/* 
	 	Update weights
	*/
	if(x+y == 0) {
		initWeights(weights, z, parameters);
	} else {
		int32_t doubleResError = ((int32_t)inputSample[offset(x,y,z,parameters)]<<1) - scaledPredicted;
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);
	}
	return computeMappedResidual(inputSample[offset(x,y,z,parameters)], scaledPredicted, parameters);
}

/* 
	CCSDS 123 Issue 1 Chapter 4.11
*/

uint16_t computeMappedResidual(uint16_t sample, uint32_t scaledPredicted, struct arguments * parameters) {	
	uint16_t predictedSample = scaledPredicted >> 1;
	int32_t delta = sample - predictedSample;
	int32_t temp1 = predictedSample - parameters->sMin;
	int32_t temp2 = parameters->sMax - predictedSample;
	int32_t omega = temp1 > temp2 ? temp2 : temp1;

	if (labs(delta) > omega) {
		return labs(delta) + omega;
	} else if ((scaledPredicted % 2 == 0 && delta >= 0) || (scaledPredicted % 2 != 0 && delta <= 0)) {
		return labs(delta)<<1;
	} else {
		return (labs(delta)<<1) - 1;
	}
}

/* 
	CCSDS 123 Issue 1 Chapter 4.7
 */

uint32_t computeScaledPredicted(uint16_t * sample, int32_t ** weightVector, int64_t * diffVector, int32_t localsum,
 	uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {

	uint32_t scaledPredicted = 0;
	if(x+y == 0) {
		if(z == 0 || parameters->precedingBands == 0) {
			scaledPredicted = parameters->sMid<<1;
		} else {
			scaledPredicted = (uint32_t)2*sample[offset(x,y,z-1,parameters)];
		}
	} else {
		int64_t diffPredicted = innerProduct(weightVector, diffVector, z, parameters);
		/// Shifting of negative numbers can cause severe issues. Thus shifting of the absolute value and then make it negative.
		int32_t tmpValue = localsum - (4L*parameters->sMid);
		int sgn = tmpValue < 0;
		tmpValue = labs(tmpValue) << parameters->weightResolution;
		tmpValue = sgn ? -1 * tmpValue : tmpValue;
		int64_t highResSample = modR(diffPredicted + tmpValue, parameters->registerSize);
		//// 
		highResSample = highResSample >> (parameters->weightResolution+1);
		highResSample += ((parameters->sMid<<1) + 1);

		int32_t lowerbounds = parameters->sMin<<1;
		int32_t higherbounds = (parameters->sMax<<1) + 1;
		scaledPredicted = clip(highResSample, lowerbounds, higherbounds);
	}
	return scaledPredicted;
}

int64_t innerProduct(int32_t ** weightVector, int64_t * diffVector, uint16_t z, struct arguments * parameters) {
	int64_t diffPredicted = 0;
	int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;
	for(int i = 0; i < currentPredictionBand; i++) {
		diffPredicted += diffVector[i] * weightVector[z][i];
	}
	if(parameters->mode == FULL) {
		for (int i = 0; i < 3; i++)
		{
			diffPredicted += diffVector[parameters->precedingBands+i] * weightVector[z][parameters->precedingBands+i];
		}
	}
	return diffPredicted;
}
