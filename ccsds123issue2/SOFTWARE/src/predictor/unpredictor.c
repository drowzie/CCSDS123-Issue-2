#include "include/predictor.h"
#include "../cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
    Requires the whole Image to be processed by decoder beforehand.
*/

uint32_t unPredict(uint32_t residuals, uint32_t * samples, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg,
int64_t * diffVector, int64_t ** weights, int16_t * absoluteError, int16_t * relativeError) {
	/*
		Calculate local sum and build up the diffrential vector at a given sample.
	*/
	uint64_t highResSample = 0;
	if(x+y != 0) {
		int64_t (*sumPtr)(uint32_t *, uint16_t, uint16_t, uint16_t, struct arguments *);
		switch (arg->sumType)
		{
		case WIDENEIGHBOR:
			sumPtr = &wideNeighborLocalSum;
			break;
		case NARROWNEIGHBOR:
			sumPtr = &narrowNeighborLocalSum;
			break;
		case WIDECOLUMN:
			sumPtr = &wideColumnLocalSum;
			break;
		case NARROWCOLUMN:
			sumPtr = &narrowColumnLocalSum;
			break;
		default:
			sumPtr = NULL;
			break;
		}
		BuildDiffVector(samples, diffVector, x, y, z, arg, sumPtr);
		highResSample = computeHighResPredSample(weights, diffVector, x, y, z, (*sumPtr)(samples, x, y, z, arg), arg);
	}
	/*
		Step for calculating prediction sample and doubleResPredSample
	*/
	uint64_t doubleResPredSample = 0; // Calculated inside function computePredictedSample
	uint64_t predictedSample = computePredictedSample(samples, &doubleResPredSample, highResSample, x, y, z, arg);
	int32_t maximumError = updateFidelityError(predictedSample, z, arg, absoluteError, relativeError);
	/*
		Decompress
	*/
	int64_t quantizerIndex = inverseMappedResidual(residuals, predictedSample, doubleResPredSample, maximumError, x, y, z, arg);
	int64_t delta = deQuantizizer(quantizerIndex, maximumError, x, y);
	uint64_t clippedBin = clippedBinCenter(predictedSample, quantizerIndex, maximumError, arg);

	if(x+y == 0) {
		initWeights(weights, z, arg);
	} else {
		int64_t doubleResError = (clippedBin << 1) - doubleResPredSample;
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, arg);
	}
	if(x+y == 0) {
		return predictedSample+delta;
	} else {
		return clippedBin;
	}
}

int64_t inverseMappedResidual(uint32_t mappedResidual, uint64_t predictedSample, uint64_t doubleResPredSample, int32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
  int64_t omega = 0;
	int64_t temp1 = predictedSample - arg->sMin;
	int64_t temp2 = arg->sMax - predictedSample;
	if (x == 0 && y == 0) {
		omega = temp1 > temp2 ? temp2 : temp1;
	} else {
		temp1 = (temp1 + (int64_t)maximumError);
		temp1 = temp1 / (2LL*(int64_t)maximumError + 1);

		temp2 = (temp2 + (int64_t)maximumError);
		temp2 = temp2 / (2LL*(int64_t)maximumError + 1);

		omega = temp1 > temp2 ? temp2 : temp1;
	}

	if(mappedResidual > (omega<<1)) {
		int sgn = sgnPlus((int64_t)predictedSample-(int64_t)arg->sMid);
		return (omega-(int64_t)mappedResidual)*sgn;
	} else {
		if((mappedResidual+doubleResPredSample) % 2 == 0) {
			return (((int64_t)mappedResidual+1)>> 1);
		} else {
			return -1 * (((int64_t)mappedResidual+1)>> 1);
		}
	}
}

/*
	Compute the inverse of quantizerIndex => delta = 2*m_z(t)*q_z(t) + q_z(t) +- m_z(t)
	notice the last term is not included as it is not possible to inverse it.
*/
int64_t deQuantizizer(int64_t quantizerIndex, int32_t maximumError, uint16_t x, uint16_t y) {
	if(x+y == 0) {
		return quantizerIndex;
	} else {
		return ((int64_t)maximumError * quantizerIndex * 2LL) + quantizerIndex;
	}
}
