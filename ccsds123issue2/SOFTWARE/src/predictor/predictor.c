#include "include/predictor.h"
#include "cliparser/include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


uint32_t predict(uint32_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg, uint32_t * sampleRep,
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
		BuildDiffVector(sampleRep, diffVector, x, y, z, arg, sumPtr);
		highResSample = computeHighResPredSample(weights, diffVector, x, y, z, (*sumPtr)(sampleRep, x, y, z, arg), arg);
	}
	/*
		Step for calculating prediction sample and doubleResPredSample
	*/
	uint64_t doubleResPredSample = 0; // Calculated inside function computePredictedSample
	uint64_t predictedSample = computePredictedSample(inputSample, &doubleResPredSample, highResSample, x, y, z, arg);
	int32_t maximumError = updateFidelityError(predictedSample, z, arg, absoluteError, relativeError);
	/*
		Quantization/Sample
	*/
	int64_t quantizerIndex = quantization(inputSample, predictedSample, maximumError, x, y, z, arg);
	uint64_t clippedBin = clippedBinCenter(predictedSample, quantizerIndex, maximumError, arg);

	sampleRep[offset(x,y,z, arg)] = sampleRepresentation(inputSample, clippedBin, predictedSample, quantizerIndex, maximumError, highResSample, x, y, z, arg);

	if(x+y == 0) {
		initWeights(weights, z, arg);
	} else {
		int64_t doubleResError = (clippedBin << 1) - (doubleResPredSample);
		updateWeightVector(weights, diffVector, doubleResError, x, y, z, arg);
	}
	return computeMappedQuantizerIndex(quantizerIndex, predictedSample, doubleResPredSample, maximumError, x, y, z, arg);
}
/* Implements the FidelityControl method as described in chapter 4.8.2 in CCSDS 123 Issue 2 Blue Book */

int32_t updateFidelityError(uint32_t predictedSample, uint16_t z, struct arguments * arg, int16_t * absoluteError, int16_t * relativeError) {
	if (arg->fidelityControl == ABSOLUTE) {
		return absoluteError[z];
	} else if (arg->fidelityControl == RELATIVE) {
		return (relativeError[z] * llabs((int64_t)predictedSample)) >> arg->dynamicRange;
	} else if (arg->fidelityControl == ABSREL) {
		uint32_t relative = (relativeError[z] * llabs((int64_t)predictedSample)) >> arg->dynamicRange;
		if(absoluteError[z] > relative) {
			return relative;
		} else {
			return absoluteError[z];
		}
	} else {
		return 0;
	}
}


/*
	CCSDS 123 Issue 2 Chapter 4.11
 */

uint32_t computeMappedQuantizerIndex(int64_t quantizerIndex, uint64_t predictedSample, uint64_t doubleResPredSample, int32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
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
	if (llabs(quantizerIndex) > omega) {
		return llabs(quantizerIndex) + omega;
	} else if ((doubleResPredSample % 2 == 0 && quantizerIndex >= 0) || (doubleResPredSample % 2 != 0 && quantizerIndex <= 0)) {
		return llabs(quantizerIndex)<<1;
	} else {
		return (llabs(quantizerIndex) << 1) - 1;
	}
}


int64_t quantization(uint32_t * sample, uint64_t predictedSample, int32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
	int64_t predictionResidual = sample[offset(x,y,z,arg)] - predictedSample;
	if (x+y==0) {
		return predictionResidual;
	} else {
		int64_t quant = llabs(predictionResidual) + maximumError;
		quant = quant / (2LL*maximumError + 1LL);
		return sgn(predictionResidual) * quant;
	}
}

uint64_t computePredictedSample(uint32_t * sample, uint64_t * doubleResPredSample, uint64_t highResPredSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
	if(x+y == 0) {
		if(z == 0 || arg->precedingBands == 0) {
			(*doubleResPredSample) = arg->sMid<<1;
		} else {
			(*doubleResPredSample) = (uint64_t)sample[offset(x,y,z-1,arg)]<<1;
		}
	} else {
		(*doubleResPredSample) = highResPredSample >> (arg->weightResolution+1);
	}
	return (*doubleResPredSample)>>1;
}

uint64_t clippedBinCenter(uint64_t predictedSample, int64_t quantizedSample, int32_t maximumError, struct arguments * arg) {
	int64_t clippedBin = quantizedSample*(2LL*maximumError + 1LL);
	return clip((int64_t)predictedSample + clippedBin,  (int64_t)arg->sMin, (int64_t)arg->sMax);
}

uint32_t sampleRepresentation(uint32_t * sample, uint64_t clippedBinCenter, uint64_t predictedSample, int64_t quantizedSample, int32_t maximumError, uint64_t highResPredSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
    if( x == 0 && y == 0) {
        return sample[offset(x,y,z,arg)];
    } else {
        uint64_t doubleResSample = 0;
        doubleResSample = (((1LL << arg->theta) - arg->sampleDamping) <<2) * ((clippedBinCenter << arg->weightResolution) - ((sgn(quantizedSample) * maximumError * arg->sampleOffset) << (arg->weightResolution - arg->theta)));
        doubleResSample += ((arg->sampleDamping * highResPredSample) - (arg->sampleDamping << (arg->weightResolution + 1)));
        doubleResSample = doubleResSample >> (arg->weightResolution + arg->theta + 1);
		doubleResSample = (doubleResSample + 1) >> 1;
		int test = 0;
        return doubleResSample;
    }
}

uint64_t computeHighResPredSample(int64_t ** weightVector, int64_t * diffVector, uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * arg) {
	int64_t predictedSample = 0;
	int64_t diffPredicted = innerProduct(weightVector, diffVector, z, arg);
	int64_t tmpValue = localsum - (int64_t)(arg->sMid<<2);
	tmpValue = tmpValue << arg->weightResolution;
	predictedSample = modR(diffPredicted + tmpValue, arg->registerSize);
	predictedSample += (arg->sMid << (arg->weightResolution + 2));
	predictedSample += (1LL << (arg->weightResolution + 1));

	int64_t lowerbounds = arg->sMin<<(arg->weightResolution+2);
	int64_t higherbounds = (arg->sMax<<(arg->weightResolution+2))+(1ULL<<(arg->weightResolution+1));
	return clip(predictedSample, lowerbounds, higherbounds);
}

int64_t innerProduct(int64_t ** weightVector, int64_t * diffVector, uint16_t z, struct arguments * arg) {
	int64_t diffPredicted = 0;
	int currentPredictionBand = z < arg->precedingBands ? z : arg->precedingBands;
	for(int i = 0; i < currentPredictionBand; i++) {
		diffPredicted += diffVector[i] * weightVector[z][i];
	}
	if(arg->mode == FULL) {
		for (int i = 0; i < 3; i++)
		{
			diffPredicted += diffVector[arg->precedingBands+i] * weightVector[z][arg->precedingBands+i];
		}
	}
	return diffPredicted;
}
