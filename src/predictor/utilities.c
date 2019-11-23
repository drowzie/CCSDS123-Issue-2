#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

long long modR(long long x, long long R){
    long long power2 = ((long long)0x1) << (R - 1);
    return ((x + power2) - (((((x + power2) >> (R - 1) >> 1)) << (R - 1)) << 1)) - power2;
}

void printVectors(long * vector, struct arguments * parameters) {
	if(parameters->mode == FULL) {
		printf("N DIFFRENCE: %ld \n", vector[parameters->precedingBands]);
		printf("W DIFFRENCE: %ld \n", vector[parameters->precedingBands+1]);
		printf("NW DIFFRENCE: %ld \n", vector[parameters->precedingBands+2]);
	}
	printf("Central Diffrences: ");
	for (int i = 0; i < parameters->precedingBands; i++)
	{
		printf("%ld ", vector[i]);
	}
	printf("\n");

}

int offset(int x, int y, int z, struct arguments * args) {
    return (z * args->xSize * args->ySize) + (y * args->xSize) + x;
}

int sgn(long long x) {
    if (x > 0) {
        return 1;
    } else if(x < 0) {
        return -1;
    } else {
        return 0;
    }
}

long long clip( long long x, long long xmin, long long xmax)  {
    if (x > xmax) {
        return xmax;
    } else if(x < xmin) {
        return xmin;
    } else {
        return x;
    }
}
