#pragma once
#include "../../cliparser/include/cli.h"

void predict(struct arguments * args, int * inputFile, unsigned short int * residuals);
int offset(int x, int y, int z, struct arguments * args);
long long computeHighResPredSample(int * localSum, int ** weightVector, int ** diffVector, unsigned int smid,  unsigned int smin, unsigned int smax, int x, int y, int z, struct arguments * parameters);

long long innerProduct(int ** weightVector, int ** diffVector, int z, struct arguments * parameters);

int computePredictedSample(int * sample, long long * doubleResPredSample, int * localSum, int ** weightVector, int ** diffVector, long long highResPredSample, unsigned int smid,  unsigned int smin, unsigned int smax, int x, int y, int z, struct arguments * parameters);

int sampleRepresentation(int * sample, int * clippedBinCenter, int predictedSample, int quantizedSample, int maximumError, int highResPredSample, int sampleDamping, int sampleOffset, int x, int y, int z, int smin, int smax, struct arguments * parameters);

int quantization(int * sample, int predictedSample, int maximumError, int x, int y, int z, struct arguments * parameters);

unsigned int computeMappedQuantizerIndex(int quantizerIndex, int predictedSample, int doubleResPredSample, int smin, int smax, int maximumError, int x, int y, int z, struct arguments * parameters);