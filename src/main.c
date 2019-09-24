
// Config file
#include "config/include/config.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "utils/include/utilities.h"
#include "predictor/include/predictor.h"


void insertTestData(int * sample){
	sample[offset(0,0,0)] = 125;
	sample[offset(1,0,0)] = 128;
	sample[offset(2,0,0)] = 133;

	sample[offset(0,1,0)] = 126;
	sample[offset(1,1,0)] = 126;
	sample[offset(2,1,0)] = 133;

	sample[offset(0,2,0)] = 127;
	sample[offset(1,2,0)] = 127;
	sample[offset(2,2,0)] = 132;

	//
	sample[offset(0,0,1)] = 225;
	sample[offset(1,0,1)] = 228;
	sample[offset(2,0,1)] = 233;

	sample[offset(0,1,1)] = 226;
	sample[offset(1,1,1)] = 226;
	sample[offset(2,1,1)] = 233;

	sample[offset(0,2,1)] = 227;
	sample[offset(1,2,1)] = 227;
	sample[offset(2,2,1)] = 232;
}

void printArray(int * sample) {
	for (int z = 0; z < zSize; z++)
	{
		printf("Z=%d\n\n", z);
		for (int y = 0; y < ySize; y++)
		{
			for (int x = 0; x < xSize; x++)
			{
				printf("%d ", sample[offset(x,y,z)]);
			}
			printf("\n");
		}
	}
}


int main(int argc, char const *argv[])
{
	int * sample = (int*) malloc(xSize*ySize*zSize*sizeof(int));
	int * localsum = (int*) malloc(xSize*ySize*zSize*sizeof(int));
	insertTestData(sample);
	wideNeighborLocalSum(sample, localsum);
	printArray(sample);
	printf("\nLOCAL SUM ARRAY\n\n");
	printArray(localsum);
	printf("%lld\n", modR(5,32));
	return 0;
}
