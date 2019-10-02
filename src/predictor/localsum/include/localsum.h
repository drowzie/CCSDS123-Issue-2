#pragma once
#include "../../../cliparser/include/cli.h"
// Local Sum Chapter 4.4
void wideNeighborLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
void narrowNeighborLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * parameters);
void wideColumnLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
void narrowColumnLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * parameters);
