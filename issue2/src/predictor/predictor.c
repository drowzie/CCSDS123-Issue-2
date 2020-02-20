#include "include/predictor.h"
#include "cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

uint32_t predict(uint32_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, uint32_t * sampleRep, 
int32_t * diffVector, int32_t * weights, uint32_t maximumError, uint32_t sampleDamping, uint32_t sampleOffset, uint32_t interbandOffset, int32_t intrabandExponent) {
	/* 
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	int64_t highResSample = 0;
	if(x+y != 0) {
		BuildDiffVector(inputSample, diffVector, x, y, z, parameters, wideNeighborLocalSum);
		highResSample = computeHighResPredSample(weights, diffVector, x, y, z, wideNeighborLocalSum(inputSample, x, y, z, parameters), parameters);
	}
	/* 
		Step for calculating prediction sample and doubleResPredSample
	*/
	int64_t doubleResPredSample = 0; // Calculated inside function computePredictedSample
	int64_t predictedSample = computePredictedSample(inputSample, &doubleResPredSample, highResSample, x, y, z, parameters);
	/* 
		Quantization/Sample "compression" part
	*/
	int32_t quantizerIndex = quantization(inputSample, predictedSample, maximumError, x, y, z, parameters);
	int32_t clippedBin = clippedBinCenter(predictedSample, quantizerIndex, maximumError, parameters);

	sampleRep[offset(x,y,z, parameters)] = sampleRepresentation(inputSample, clippedBin, predictedSample, quantizerIndex, maximumError, highResSample, sampleDamping, sampleOffset, x, y, z, parameters);
	if(x+y == 0) {
		initWeights(weights, z, parameters);
	} else {
		int64_t doubleResError = (clippedBin << 1) - doubleResPredSample;
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, interbandOffset, intrabandExponent, parameters);
	}
	uint32_t residual = computeMappedQuantizerIndex(quantizerIndex, predictedSample, doubleResPredSample, maximumError, x, y, z, parameters);
	int32_t inverse = inverseMappedResidual(residual, predictedSample, doubleResPredSample, maximumError, x, y, z, parameters);

	if(inverse != quantizerIndex) {
		printf("Inverse: %d != Residual: %d \n", inverse, quantizerIndex);
		exit(0);
	}

	return residual;
}


/* 
	CCSDS 123 Issue 2 Chapter 4.11
 */

uint32_t computeMappedQuantizerIndex(int32_t quantizerIndex, int64_t predictedSample, int64_t doubleResPredSample, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {	
	int64_t omega = 0;
	int64_t temp1 = predictedSample - parameters->sMin;
	int64_t temp2 = parameters->sMax - predictedSample;
	
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

int64_t computePredictedSample(uint32_t * sample, int64_t * doubleResPredSample, int64_t highResPredSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
	if(x+y == 0) {
		if(z == 0 || parameters->precedingBands == 0) {
			(*doubleResPredSample) = parameters->sMid << 1;
		} else {
			(*doubleResPredSample) = sample[offset(x,y,z-1,parameters)] << 1;
		}
	} else {
		(*doubleResPredSample) = highResPredSample >> (parameters->weightResolution+1);
	}
	return (*doubleResPredSample) >> 1;
}

int32_t clippedBinCenter(int32_t predictedSample, int32_t quantizedSample, uint32_t maximumError, struct arguments * parameters) {
	return clip(predictedSample + (quantizedSample*((maximumError << 1) + 1)), parameters->sMin, parameters->sMax);
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

int64_t computeHighResPredSample(int32_t * weightVector, int32_t * diffVector, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	int32_t diffPredicted = 0;
	int64_t predictedSample = 0;
	diffPredicted = innerProduct(weightVector, diffVector, z, parameters);
	///////
	int32_t tmpValue = localsum - (parameters->sMid << 2);
	int sgn = tmpValue < 0;
	tmpValue = abs(tmpValue) << parameters->weightResolution;
	tmpValue = sgn ? -1 * tmpValue : tmpValue;
	predictedSample = modR(diffPredicted + tmpValue, parameters->registerSize);
	////
	predictedSample += (parameters->sMid << (parameters->weightResolution + 2));
	predictedSample += (1 << (parameters->weightResolution + 1));

	int32_t lowerbounds = (parameters->sMin<<(parameters->weightResolution+2));
	int32_t higherbounds = (parameters->sMax<<(parameters->weightResolution+2) + (1 << (parameters->weightResolution+1)));
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
