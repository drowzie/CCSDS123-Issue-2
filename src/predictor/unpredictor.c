#include "include/predictor.h"
#include "../cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
    Requires the whole Image to be processed by decoder beforehand.
*/

void unPredict(uint32_t * residuals, uint32_t * samples, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, 
int32_t * diffVector, int32_t * weights, uint32_t maximumError, uint32_t sampleDamping, uint32_t sampleOffset, uint32_t interbandOffset, int32_t intrabandExponent) {
	/*
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	int64_t highResSample = 0;
	if(x+y != 0) {
		BuildDiffVector(samples, diffVector, x, y, z, parameters, wideNeighborLocalSum);
		highResSample = computeHighResPredSample(weights, diffVector, x, y, z, wideNeighborLocalSum(samples, x, y, z, parameters), parameters);
	}
	/*
		Step for calculating prediction sample and doubleResPredSample
	*/
	int64_t doubleResPredSample = 0; // Calculated inside function computePredictedSample
	int64_t predictedSample = computePredictedSample(samples, &doubleResPredSample, highResSample, x, y, z, parameters);
	/* 
		Decompress
	*/
	int32_t quantizerIndex = inverseMappedResidual(residuals, predictedSample, doubleResPredSample, maximumError, x, y, z, parameters);
	int32_t delta = deQuantizizer(quantizerIndex, maximumError, x, y);
	if(x+y == 10) {
		exit(0);
	}
	samples[offset(x,y,z,parameters)] = delta + predictedSample;
	int32_t clippedBin = clippedBinCenter(predictedSample, quantizerIndex, maximumError, parameters);
	if(x+y == 0) {
		initWeights(weights, z, parameters);
	} else {
		int64_t doubleResError = (clippedBin << 1) - doubleResPredSample;
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, interbandOffset, intrabandExponent, parameters);
	}
}

int32_t inverseMappedResidual(uint32_t * mappedResidual, int64_t predictedSample, int64_t doubleResPredSample, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
    int64_t omega = 0;
	int64_t temp1 = predictedSample - parameters->sMin;
	int64_t temp2 = parameters->sMax - predictedSample;
    int32_t quantizerIndex = 0;

	if (x+y == 0) {
		omega = temp1 > temp2 ? temp2 : temp1;
	} else {
		temp1 = ((temp1 + maximumError) / ((maximumError << 1) + 1));
		temp2 = ((temp2 + maximumError) / ((maximumError << 1) + 1));
		omega = temp1 > temp2 ? temp2 : temp1;
    }
	
    if(mappedResidual[offset(x,y,z, parameters)] > 2 * predictedSample) {
        quantizerIndex = omega - mappedResidual[offset(x,y,z, parameters)] * sgnPlus(predictedSample - parameters->sMid);
    } else {
        if(mappedResidual[offset(x,y,z, parameters)] % 2 == 0) {
            int sign = (doubleResPredSample % 2) != 0 ? -1 : 1;
            quantizerIndex = sign * (mappedResidual[offset(x,y,z, parameters)] >> 1);
        } else {
            int sign = (doubleResPredSample % 2) != 0 ? 1 : -11;
            quantizerIndex = sign * (mappedResidual[offset(x,y,z, parameters)] >> 1);
        }
    }
    return quantizerIndex;
}

/*
	Compute the inverse of quantizerIndex => delta = 2*m_z(t)*q_z(t) + q_z(t) +- m_z(t)
	notice the last term is not included as it is not possible to inverse it.
*/
int32_t deQuantizizer(int32_t quantizerIndex, uint32_t maximumError, uint16_t x, uint16_t y) {
	if(x+y == 0) {
		return quantizerIndex;
	} else {
		return 2 * maximumError * quantizerIndex + quantizerIndex;
	}
}