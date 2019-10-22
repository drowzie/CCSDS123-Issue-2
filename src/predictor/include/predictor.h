#pragma once
#include "../../cliparser/include/cli.h"

void predict(struct arguments * args, unsigned int * inputFile, unsigned long * residuals);
int offset(int x, int y, int z, struct arguments * args);
long long computeHighResPredSample(unsigned int * localSum, long ** weightVector, int ** diffVector, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters);

long long innerProduct(long ** weightVector, int ** diffVector, int z, struct arguments * parameters);

long long computePredictedSample(unsigned int * sample, long long * doubleResPredSample, int * localSum, long ** weightVector, int ** diffVector, long long highResPredSample, long smid, long smin, long smax, int x, int y, int z, struct arguments * parameters);

unsigned int sampleRepresentation(unsigned int * sample, long long * clippedBinCenter, long predictedSample, long quantizedSample, int maximumError, long long highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, long long smin, long long smax, struct arguments * parameters);

long quantization(unsigned int * sample, long long predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters);

long computeMappedQuantizerIndex(long quantizerIndex, long long predictedSample, long long doubleResPredSample, long smin, long smax, int maximumError, int x, int y, int z, struct arguments * parameters);
