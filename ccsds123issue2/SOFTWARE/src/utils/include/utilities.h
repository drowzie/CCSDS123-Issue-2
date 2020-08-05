#pragma once
#include "../../cliparser/include/cli.h"
#include <stdio.h>
int readIntSamples(struct arguments * parameters, char fileName[128], uint32_t * samples);
double walltime ( void );
