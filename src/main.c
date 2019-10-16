
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cliparser/include/cli.h"
#include "predictor/include/predictor.h"


void insertTestData(int * sample, struct arguments * args){
	sample[offset(0,0,0, args)] = 2;
	sample[offset(1,0,0, args)] = 4;
	sample[offset(2,0,0, args)] = 6;
	sample[offset(3,0,0, args)] = 9;

	sample[offset(0,1,0, args)] = 3;
	sample[offset(1,1,0, args)] = 4;
	sample[offset(2,1,0, args)] = 5;
	sample[offset(3,1,0, args)] = 5;

	sample[offset(0,2,0, args)] = 6;
	sample[offset(1,2,0, args)] = 7;
	sample[offset(2,2,0, args)] = 7;
	sample[offset(3,2,0, args)] = 8;
 
	sample[offset(0,3,0, args)] = 6;
	sample[offset(1,3,0, args)] = 7;
	sample[offset(2,3,0, args)] = 7;
	sample[offset(3,3,0, args)] = 7;

	//
	sample[offset(0,0,1, args)] = 3;
	sample[offset(1,0,1, args)] = 4;
	sample[offset(2,0,1, args)] = 5;
	sample[offset(3,0,1, args)] = 5;

	sample[offset(0,1,1, args)] = 6;
	sample[offset(1,1,1, args)] = 2;
	sample[offset(2,1,1, args)] = 1;
	sample[offset(3,1,1, args)] = 1;

	sample[offset(0,2,1, args)] = 3;
	sample[offset(1,2,1, args)] = 4;
	sample[offset(2,2,1, args)] = 5;
	sample[offset(3,2,1, args)] = 5;

	sample[offset(0,3,1, args)] = 3;
	sample[offset(1,3,1, args)] = 4;
	sample[offset(2,3,1, args)] = 5;
	sample[offset(3,3,1, args)] = 5;
}



int main(int argc, char **argv)
{
	/*
		Parse arguments. --h to see what available arguments(declared in cli.c)
	*/

	struct arguments args;
	parseArguments(argc, argv, &args);

	// Create Test Data
	unsigned int * sample = (int*) malloc(args.xSize*args.ySize*args.zSize*sizeof(unsigned int));
	unsigned int * residuals = (int*) malloc(args.xSize*args.ySize*args.zSize*sizeof(int));
	insertTestData(sample, &args);
	predict(&args,sample,residuals);


	free(sample);

	return 0;
}
