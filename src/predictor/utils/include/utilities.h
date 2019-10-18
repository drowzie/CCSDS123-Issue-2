#pragma once
#include "../../../cliparser/include/cli.h"

#define sgnplus(x) ((x) >= (0) ? 1 : -1)

long long clip(long long x, long long xmin, long long xmax);


long long modR(long long x, long long R);
void printArray(unsigned int * sample, struct arguments * args);
void printVectors(int ** vector, struct arguments * parameters);
int offset(int x, int y, int z, struct arguments * args);
int sgn(long long x);