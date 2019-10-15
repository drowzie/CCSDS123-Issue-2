#pragma once
#include "../../../cliparser/include/cli.h"

void initWeights(long ** weights, int z, struct arguments * parameters);
void updateWeightVector(long ** weights, int ** diffVector, long long error, int x, int y, int z, struct arguments * parameters);