#pragma once
#include "cliparser/include/cli.h"
#include <inttypes.h>

/* 
    MACROS
*/
#define clip(x, xmin, xmax) ((x) < (xmin) ? (xmin) : ((x) > (xmax) ? (xmax) : (x)))



/*
    Functions for prediction and parts of decompression
*/
uint32_t predict(uint32_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, uint32_t * sampleRep,
    int64_t * diffVector, int64_t ** weights, int16_t * absoluteError, int16_t * relativeError);

uint64_t computeHighResPredSample( int64_t ** weightVector, int64_t * diffVector, uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * parameters);
int64_t innerProduct(int64_t ** weightVector, int64_t * diffVector, uint16_t z, struct arguments * parameters);
uint64_t computePredictedSample(uint32_t * sample, uint64_t * doubleResPredSample,  uint64_t highResPredSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
uint64_t clippedBinCenter(uint64_t predictedSample, int64_t quantizedSample, int32_t maximumError, struct arguments * parameters);
uint32_t sampleRepresentation(uint32_t * sample, uint64_t clippedBinCenter, uint64_t predictedSample, int64_t quantizedSample, int32_t maximumError, uint64_t highResPredSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int64_t quantization(uint32_t * sample, uint64_t predictedSample, int32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
uint32_t computeMappedQuantizerIndex(int64_t quantizerIndex, uint64_t predictedSample, uint64_t doubleResPredSample, int32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t updateFidelityError(uint32_t predictedSample, uint16_t z, struct arguments * parameters, int16_t * absoluteError, int16_t * relativeError);
/*
    Functions for decompression
*/
uint32_t unPredict(uint32_t residuals, uint32_t * samples, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters,
                  int64_t * diffVector, int64_t ** weights, int16_t * absoluteError, int16_t * relativeError);

int64_t inverseMappedResidual(uint32_t  mappedResidual, uint64_t predictedSample, uint64_t doubleResPredSample, int32_t maximumError,
    uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

int64_t deQuantizizer(int64_t quantizerIndex, int32_t maximumError, uint16_t x, uint16_t y);

// Local Diffrences Chapter 4.5
int64_t centralLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * parameters);
int64_t northLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * parameters);
int64_t westLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * parameters);
int64_t northwestLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * parameters);
void BuildDiffVector(uint32_t * sample,  int64_t * diffVector, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int64_t(*localSumFunc)(uint32_t *, uint16_t, uint16_t, uint16_t, struct arguments *));

//WEIGHTS
void initWeights(int64_t ** weights, uint16_t z, struct arguments * parameters);
void updateWeightVector(int64_t ** weights, int64_t * diffVector, int64_t error, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

// Local Sum Chapter 4.4
int64_t wideNeighborLocalSum(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int64_t narrowNeighborLocalSum(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int64_t wideColumnLocalSum(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int64_t narrowColumnLocalSum(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

// UTILS
int offset(uint16_t x, uint16_t y, uint16_t z, struct arguments * args);
int64_t modR(int64_t x, int64_t R);
int sgn(int64_t x);
int sgnPlus(int64_t x);
uint64_t absoluteValue(int64_t x );