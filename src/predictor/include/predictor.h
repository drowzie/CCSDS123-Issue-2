#pragma once
#include "../../cliparser/include/cli.h"
#include <inttypes.h>

uint32_t predict(uint32_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, uint32_t * sampleRep,  
    int32_t * diffVector, int32_t * weights, uint32_t maximumError, uint32_t sampleDamping, uint32_t sampleOffset, uint32_t interbandOffset, int32_t intrabandExponent);

int64_t computeHighResPredSample( int32_t * weightVector, int32_t * diffVector, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int32_t innerProduct(int32_t * weightVector, int32_t * diffVector, uint16_t z, struct arguments * parameters);
int64_t computePredictedSample(uint32_t * sample, int64_t * doubleResPredSample,  int64_t highResPredSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t clippedBinCenter(int32_t predictedSample, int32_t quantizedSample, uint32_t maximumError, struct arguments * parameters);
uint32_t sampleRepresentation(uint32_t * sample, int32_t clippedBinCenter, int32_t predictedSample, int32_t quantizedSample, uint32_t maximumError, int64_t highResPredSample, int sampleDamping, int sampleOffset, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t quantization(uint32_t * sample, int64_t predictedSample, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t computeMappedQuantizerIndex(int32_t quantizerIndex, int64_t predictedSample, int64_t doubleResPredSample, uint32_t maximumError, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

// Local Diffrences Chapter 4.5
int centralLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int northLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int westLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int northwestLocalDiffrence(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
void BuildDiffVector(uint32_t * sample,  int32_t * diffVector, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int32_t(*localSumFunc)(uint32_t *, uint16_t, uint16_t, uint16_t, struct arguments *));

//WEIGHTS
void initWeights(int32_t * weights, uint16_t z, struct arguments * parameters);
void updateWeightVector(int32_t * weights, int32_t * diffVector, int64_t error, uint16_t x, uint16_t y, uint16_t z, int interbandOffset, int intrabandExponent, struct arguments * parameters);

// Local Sum Chapter 4.4
int32_t wideNeighborLocalSum(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t narrowNeighborLocalSum(uint32_t * sample,  int32_t smid, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t wideColumnLocalSum(uint32_t * sample,  uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t narrowColumnLocalSum(uint32_t * sample,  int32_t smid, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

// UTILS

int offset(uint16_t x, uint16_t y, uint16_t z, struct arguments * args);
int64_t clip(int64_t x, int64_t xmin, int64_t xmax);
int64_t modR(int64_t x, int64_t R);
int sgn(int64_t x);
