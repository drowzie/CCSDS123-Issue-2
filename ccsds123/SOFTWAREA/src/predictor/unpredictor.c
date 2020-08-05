#include "predictor.h"
#include "cliparser/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
    Requires the whole Image to be processed by decoder beforehand.
*/

uint32_t unPredict(uint16_t * inputSample, uint16_t mappedResidual, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int64_t * diffVector, int32_t ** weights) {
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
	int32_t delta = inverseMappedResidual(mappedResidual, scaledPredicted>>1, scaledPredicted, parameters);
	uint16_t sample = (scaledPredicted>>1) + delta;
	/* 
	 	Update weights
	*/
	if(x+y == 0) {
		initWeights(weights, z, parameters);
	} else {
		int32_t doubleResError = (int32_t)((sample << 1) - scaledPredicted);
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);
	}
	return sample;
}

int32_t inverseMappedResidual(uint32_t mappedResidual, uint64_t predictedSample, uint32_t scaledPredicted, struct arguments * parameters) {
    int64_t omega = 0;
	int64_t temp1 = predictedSample - parameters->sMin;
	int64_t temp2 = parameters->sMax - predictedSample;

    omega = temp1 > temp2 ? temp2 : temp1;

	if(mappedResidual > (2*omega)) {
		int sgn = sgnPlus((int64_t)predictedSample-parameters->sMid);
		return ((int64_t)omega-mappedResidual)*sgn;
	} else {
		if((mappedResidual+scaledPredicted) % 2 == 0) {
			return ((mappedResidual+1) / 2);
		} else {
			return -1 * ((mappedResidual+1) / 2);
		}
	}
}
