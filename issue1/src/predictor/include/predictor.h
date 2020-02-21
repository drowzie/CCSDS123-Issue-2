#pragma once
#include "cliparser/include/cli.h"
#include <inttypes.h>

#define clip(x, xmin, xmax) ((x) < (xmin) ? (xmin) : ((x) > (xmax) ? (xmax) : (x)))

/* 
    Functions for prediction and parts of decompression
*/
uint16_t predict(uint16_t * inputSample, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int32_t * diffVector, int32_t * weights);

int32_t innerProduct(int32_t * weightVector, int32_t * diffVector, uint16_t z, struct arguments * parameters);
uint32_t computeScaledPredicted(uint16_t * sample, int32_t * weightVector, int32_t * diffVector, int32_t localsum, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
uint16_t computeMappedResidual(uint16_t sample, uint32_t scaledPredicted, struct arguments * parameters);

// Local Diffrences Chapter 4.5
int32_t centralLocalDiffrence(uint16_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int32_t northLocalDiffrence(uint16_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int32_t westLocalDiffrence(uint16_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
int32_t northwestLocalDiffrence(uint16_t * sample,  uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters);
void BuildDiffVector(uint16_t * sample,  int32_t * diffVector, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int32_t(*localSumFunc)(uint16_t *, uint16_t, uint16_t, uint16_t, struct arguments *));

// CCSDS 123 Issue 1 Weight update and init. Chapter 4.6 + Chapter 4.8
void initWeights(int32_t * weights, uint16_t z, struct arguments * parameters);
void updateWeightVector(int32_t * weights, int32_t * diffVector, int32_t error, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

// Local Sum Chapter 4.4
int32_t wideNeighborLocalSum(uint16_t * sample,  uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);
int32_t wideColumnLocalSum(uint16_t * sample,  uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters);

// UTILS
int offset(uint16_t x, uint16_t y, uint16_t z, struct arguments * args);
int64_t modR(int64_t x, int64_t R);
int sgn(int64_t x);

