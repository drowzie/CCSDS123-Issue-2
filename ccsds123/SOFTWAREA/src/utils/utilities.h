#pragma once
#include "cliparser/cli.h"
#include <stdio.h>
int readIntSamples(struct arguments * parameters, char fileName[128], uint16_t * samples);
void printArrayLong(unsigned long * sample, struct arguments * args);
double walltime ( void );
void writeArrayLong(unsigned long * sample, FILE * file, struct arguments * args);