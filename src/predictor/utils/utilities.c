#include "include/utilities.h"
#include "../../cliparser/include/cli.h"
#include <stdio.h>

long long modR(long long x, long long R){
    long long power2 = ((long long)0x1) << (R - 1);
    return ((x + power2) - (((((x + power2) >> (R - 1) >> 1)) << (R - 1)) << 1)) - power2;
}

void printArray(int * sample, struct arguments * args) {
	for (int z = 0; z < args->zSize; z++)
	{
		printf("Z=%d\n\n", z);
		for (int y = 0; y < args->ySize; y++)
		{
			for (int x = 0; x < args->xSize; x++)
			{
				printf("%d ", sample[offset(x,y,z,args)]);
			}
			printf("\n");
		}
	}
}

int offset(int x, int y, int z, struct arguments * args) { 
    return (z * args->xSize * args->ySize) + (y * args->xSize) + x; 
}