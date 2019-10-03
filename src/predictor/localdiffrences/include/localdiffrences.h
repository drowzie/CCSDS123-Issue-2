#include "../../../cliparser/include/cli.h"
// Local Diffrences Chapter 4.5
int centralLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
int northLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
int westLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);
int northwestLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters);

void BuildDiffVector(int * sample, int * localsum, int ** diffVector, int x, int y, int z, struct arguments * parameters);