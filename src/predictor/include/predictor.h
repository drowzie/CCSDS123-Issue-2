#pragma once
#include "../../cliparser/include/cli.h"

unsigned long predict(unsigned long * inputSample,
    int x, int y, int z,
    struct arguments * parameters,
    unsigned long * sampleRep,
    long * localsum,
    long * diffVector,
    long * weights,
    long sMin, long sMax, long sMid, 
    int maximumError, int sampleDamping, int sampleOffset,
    int interbandOffset, int intrabandExponent);







long long computeHighResPredSample(long * localsum, long * weightVector, long * diffVector, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters);
long innerProduct(long * weightVector, long * diffVector, int z, struct arguments * parameters);
long long computePredictedSample(unsigned long * sample, long long * doubleResPredSample, long * localsum, long long highResPredSample, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters);
long clippedBinCenter(long predictedSample, long quantizedSample, int maximumError, long sMin, long sMax);
unsigned int sampleRepresentation(unsigned long * sample, long clippedBinCenter, long predictedSample, long quantizedSample, int maximumError, long long highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, struct arguments * parameters);
long quantization(unsigned long * sample, long long predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters);
long computeMappedQuantizerIndex(long quantizerIndex, long long predictedSample, long long doubleResPredSample, long smin, long smax, int maximumError, int x, int y, int z, struct arguments * parameters);

enum DIFFRENCESSTATE {CENTRAL=0, NORTH=1, WEST=2, NORTHWEST=3};
extern enum DIFFRENCESSTATE DIFFRENCESSTATE;

// Local Diffrences Chapter 4.5
int centralLocalDiffrence(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters);
int northLocalDiffrence(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters);
int westLocalDiffrence(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters);
int northwestLocalDiffrence(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters);
void BuildDiffVector(unsigned long * sample, long * localsum, long * diffVector, int x, int y, int z, struct arguments * parameters);

//WEIGHTS
void initWeights(long * weights, int z, struct arguments * parameters);
void updateWeightVector(long * weights, long * diffVector, long long error, int x, int y, int z, int interbandOffset, int intrabandExponent, struct arguments * parameters);

// Local Sum Chapter 4.4
void wideNeighborLocalSum(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters);
void narrowNeighborLocalSum(unsigned long * sample, long * localsum, int smid, int x, int y, int z, struct arguments * parameters);
void wideColumnLocalSum(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters);
void narrowColumnLocalSum(unsigned long * sample, long * localsum, int smid, int x, int y, int z, struct arguments * parameters);

// UTILS

int offset(int x, int y, int z, struct arguments * args);
long long clip(long long x, long long xmin, long long xmax);
long long modR(long long x, long long R);
void printVectors(long * vector, struct arguments * parameters);
int offset(int x, int y, int z, struct arguments * args);
int sgn(long long x);
