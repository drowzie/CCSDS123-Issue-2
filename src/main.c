
// Config file
#include "config/include/config.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cliparser/include/cli.h"
#include "utils/include/utilities.h"
#include "predictor/include/predictor.h"


void insertTestData(int * sample, struct arguments * args){
	sample[offset(0,0,0, args)] = 125;
	sample[offset(1,0,0, args)] = 128;
	sample[offset(2,0,0, args)] = 133;

	sample[offset(0,1,0, args)] = 126;
	sample[offset(1,1,0, args)] = 126;
	sample[offset(2,1,0, args)] = 133;

	sample[offset(0,2,0, args)] = 127;
	sample[offset(1,2,0, args)] = 127;
	sample[offset(2,2,0, args)] = 132;

	//
	sample[offset(0,0,1, args)] = 225;
	sample[offset(1,0,1, args)] = 228;
	sample[offset(2,0,1, args)] = 233;

	sample[offset(0,1,1, args)] = 226;
	sample[offset(1,1,1, args)] = 226;
	sample[offset(2,1,1, args)] = 233;

	sample[offset(0,2,1, args)] = 227;
	sample[offset(1,2,1, args)] = 227;
	sample[offset(2,2,1, args)] = 232;
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



int main(int argc, char **argv)
{
	
	/*
		Parse arguments. --h to see what available arguments(declared in cli.c)
	*/
	struct arguments args;
	parseArguments(argc, argv, &args);
	printf("%d", args.precedingBands);


	// Create Test Data
	int * sample = (int*) malloc(args.xSize*args.ySize*args.zSize*sizeof(int));
	int * localsum = (int*) malloc(args.xSize*args.ySize*args.zSize*sizeof(int));
	insertTestData(sample, &args);

	for (int z = 0; z < args.zSize; z++)
	{
		for (int y = 0; y < args.ySize; y++)
		{
			for (int x = 0; x < args.xSize; x++)
			{
				wideNeighborLocalSum(sample, localsum, x, y , z, &args);

			}
			
		}
		
	}
	
	printArray(sample, &args);
	printf("\nLOCAL SUM ARRAY\n\n");
	//printArray(localsum, &args);
	//printf("%lld\n", modR(5,32));
	return 0;
}
