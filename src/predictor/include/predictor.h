#pragma once
#include "../../cliparser/include/cli.h"

void predict(struct arguments * args, int * inputFile, unsigned short int * residuals);
int offset(int x, int y, int z, struct arguments * args);