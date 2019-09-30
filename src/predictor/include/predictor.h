#pragma once
#include "../../cliparser/include/cli.h"

// Local Sum Chapter 4.4
void wideNeighborLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * args);
void narrowNeighborLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * args);
void wideColumnLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * args);
void narrowColumnLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * args);



// Local Diffrences Chapter 4.5
void centralLocalDiffrence(int * sample, int * localsum, int * centraldiffrence, int x, int y, int z, struct arguments * args);
void northLocalDiffrence(int * sample, int * localsum, int * northLocalDiffrence, int x, int y, int z, struct arguments * args);
void westLocalDiffrence(int * sample, int * localsum, int * westLocalDiffrence, int x, int y, int z, struct arguments * args);
void northwestLocalDiffrence(int * sample, int * localsum, int * northwestLocalDiffrence, int x, int y, int z, struct arguments * args);

int offset(int x, int y, int z, struct arguments * args);