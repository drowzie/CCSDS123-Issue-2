#include "include/utilities.h"
#include "../../cliparser/include/cli.h"
#include <stdio.h>

long long modR(long long x, long long R){
    long long power2 = ((long long)0x1) << (R - 1);
    return ((x + power2) - (((((x + power2) >> (R - 1) >> 1)) << (R - 1)) << 1)) - power2;
}

void printArray(unsigned int * sample, struct arguments * args) {
	for (int z = 0; z < args->zSize; z++)
	{
		printf("Z=%d\n\n", z);
		for (int y = 0; y < args->ySize; y++)
		{
			for (int x = 0; x < args->xSize; x++)
			{
				printf("%u ", sample[offset(x,y,z,args)]);
			}
			printf("\n");
		}
	}
}

void printVectors(int ** vector, struct arguments * parameters) {
	if(parameters->mode == FULL) {
		printf("N DIFFRENCE: %d \n", vector[1][0]);
		printf("W DIFFRENCE: %d \n", vector[2][0]);
		printf("NW DIFFRENCE: %d \n", vector[3][0]);
	}
	printf("Central Diffrences: ");
	for (int i = 0; i < parameters->precedingBands; i++)
	{
		printf("%d ", vector[0][i]);
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