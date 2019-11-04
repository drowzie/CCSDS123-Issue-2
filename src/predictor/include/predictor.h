#pragma once
#include "../../cliparser/include/cli.h"

void predict(unsigned int * inputSample,
    unsigned long * residuals,
    int x, int y, int z,
    struct arguments * parameters,
    unsigned int * sampleRep,
    int * localsum,
    int ** diffVector,
    long ** weights,
    long sMin, long sMax, long sMid, 
    int maximumError, int sampleDamping, int sampleOffset);







long long computeHighResPredSample(unsigned int * localSum, long ** weightVector, int ** diffVector, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters);
long long innerProduct(long ** weightVector, int ** diffVector, int z, struct arguments * parameters);
long long computePredictedSample(unsigned int * sample, long long * doubleResPredSample, int * localSum, long ** weightVector, int ** diffVector, long long highResPredSample, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters);
long clippedBinCenter(long predictedSample, long quantizedSample, int maximumError, long sMin, long sMax);
unsigned int sampleRepresentation(unsigned int * sample, long clippedBinCenter, long predictedSample, long quantizedSample, int maximumError, long long highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, struct arguments * parameters);
long quantization(unsigned int * sample, long long predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters);
long computeMappedQuantizerIndex(long quantizerIndex, long long predictedSample, long long doubleResPredSample, long smin, long smax, int maximumError, int x, int y, int z, struct arguments * parameters);

enum DIFFRENCESSTATE {CENTRAL=0, NORTH=1, WEST=2, NORTHWEST=3};
extern enum DIFFRENCESSTATE DIFFRENCESSTATE;

// Local Diffrences Chapter 4.5
int centralLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
int northLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
int westLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
int northwestLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
void BuildDiffVector(int * sample, int * localsum, int ** diffVector, int x, int y, int z, struct arguments * parameters);

//WEIGHTS
void initWeights(long ** weights, int z, struct arguments * parameters);
void updateWeightVector(long ** weights, int ** diffVector, long long error, int x, int y, int z, struct arguments * parameters);

// Local Sum Chapter 4.4
void wideNeighborLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
void narrowNeighborLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * parameters);
void wideColumnLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
void narrowColumnLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * parameters);

// UTILS

int offset(int x, int y, int z, struct arguments * args);
long long clip(long long x, long long xmin, long long xmax);
long long modR(long long x, long long R);
void printArrayLong(unsigned long * sample, struct arguments * args);
void printArrayInt(unsigned int * sample, struct arguments * args);
void printVectors(int ** vector, struct arguments * parameters);
int offset(int x, int y, int z, struct arguments * args);
int sgn(long long x);
