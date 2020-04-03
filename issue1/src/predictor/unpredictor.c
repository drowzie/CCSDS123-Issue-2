#include "include/predictor.h"
#include "../cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
    Requires the whole Image to be processed by decoder beforehand.
*/

uint32_t unPredict(uint16_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int32_t * diffVector, int32_t * weights) {
    /*
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	if(x+y != 0) {
		BuildDiffVector(inputSample, diffVector, x, y, z, parameters, wideNeighborLocalSum);
	}
	/* 
		Step for calculating prediction sample and scaledPredicted
	*/
	uint32_t scaledPredicted = computeScaledPredicted(inputSample, weights, diffVector, wideNeighborLocalSum(inputSample, x, y, z, parameters), x, y, z, parameters);
    
	/* 
	 	Update weights
	*/
	if(x+y == 0) {
		initWeights(weights, z, parameters);
	} else {
		int32_t doubleResError = (inputSample[offset(x,y,z,parameters)] << 1) - scaledPredicted;
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);
	}
}

int32_t inverseMappedResidual(uint32_t mappedResidual, int64_t predictedSample, int64_t doubleResPredSample, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
    int64_t omega = 0;
	int64_t temp1 = predictedSample - parameters->sMin;
	int64_t temp2 = parameters->sMax - predictedSample;

    omega = temp1 > temp2 ? temp2 : temp1;

	if(mappedResidual > (omega<<1)) {
		int sgn = sgnPlus(predictedSample-parameters->sMid);
		return (omega-mappedResidual)*sgn;
	} else {
		if((mappedResidual+doubleResPredSample) % 2 == 0) {
			return ((mappedResidual+1) / 2);
		} else {
			return -1 * ((mappedResidual+1) / 2);
		}
	}
}
