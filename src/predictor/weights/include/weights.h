#pragma once
#include "../../../cliparser/include/cli.h"

void initWeights(int ** weights, int z, struct arguments * parameters);
void updateWeightVector(int ** weights, int ** diffVector, int error, int x, int y, int z, struct arguments * parameters);