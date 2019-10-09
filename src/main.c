
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cliparser/include/cli.h"
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



int main(int argc, char **argv)
{
	/*
		Parse arguments. --h to see what available arguments(declared in cli.c)
	*/

	struct arguments args;
	parseArguments(argc, argv, &args);

	// Create Test Data
	int * sample = (int*) malloc(args.xSize*args.ySize*args.zSize*sizeof(int));
	int * residuals = (int*) malloc(args.xSize*args.ySize*args.zSize*sizeof(int));
	insertTestData(sample, &args);
	predict(&args,sample,residuals);


	free(sample);

	return 0;
}
