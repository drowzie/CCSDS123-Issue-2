#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

/*
	Chapter 4.4 in CCSDS 123 Issue 2
*/

void wideNeighborLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if (y == 0 && x > 0){
			localsum[offset(x,y,z,args)] = 4 * sample[offset(x-1,y,z, args)];
		} else if (y > 0 && x > 0 && x < args->xSize-1) {
			localsum[offset(x,y,z,args)] = sample[offset(x-1,y,z, args)] + sample[offset(x-1,y-1,z, args)] + sample[offset(x,y-1,z, args)] + sample[offset(x+1,y-1,z, args)];
		} else if (y > 0 && x == 0) {
			localsum[offset(x,y,z,args)] = 2 * (sample[offset(x+1,y-1,z, args)] + sample[offset(x,y-1,z, args)]);
		} else if (y > 0 && x == args->xSize-1) {
			localsum[offset(x,y,z,args)] = sample[offset(x-1,y,z, args)] + sample[offset(x-1,y-1,z, args)] + 2 * sample[offset(x,y-1,z, args)];
		} else {
			printf("wideNeighborLocalSum should not arrive here\n");
		}
	}
}

void narrowNeighborLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if (y > 0 && x > 0 && x < args->xSize-1)
		{
			localsum[offset(x,y,z,args)] = sample[offset(x-1,y-1,z, args)] +  (2 * sample[offset(x,y-1,z, args)]) + sample[offset(x+1,y-1,z, args)];
		} else if (y == 0 && x > 0 && y > 0) {
			localsum[offset(x,y,z,args)] = 4 * sample[offset(x-1,y,z-1, args)];
		} else if (y > 0 && x == 0) {
			localsum[offset(x,y,z,args)] = 2 * (sample[offset(x,y-1,z, args)] + sample[offset(x+1,y-1,z, args)]);
		} else if (y > 0 && x == args->xSize - 1) {
			localsum[offset(x,y,z,args)] = 2 * (sample[offset(x-1,y-1,z, args)] + sample[offset(x,y-1,z, args)]);
		} else if (y == 0 && x > 0 && z == 0) {
			localsum[offset(x,y,z,args)] = 4 * smid;
		} else {
			printf("narrowNeighborLocalSum should not arrive here\n");
		}
	}
}

void wideColumnLocalSum(int * sample, int * localsum, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if(y > 0) {
			localsum[offset(x,y,z,args)] = 4 * sample[offset(x,y-1,z, args)];
		} else if (y == 0 && x > 0) {
			localsum[offset(x,y,z,args)] = 4 * sample[offset(x-1,y,z, args)];
		} else {
			printf("wideColumnLocalSum should not arrive here\n");
		}
	}
}

void narrowColumnLocalSum(int * sample, int * localsum, int smid, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if (y > 0)
		{
			localsum[offset(x,y,z,args)] = 4 * sample[offset(x,y-1,z, args)];
		} else if (y == 0 && x > 0 && z > 0) {
			localsum[offset(x,y,z,args)] = 4 * sample[offset(x-1,y,z-1, args)];
		} else if (y == 0 && x > 0 && z == 0) {
			localsum[offset(x,y,z,args)] = 4 * smid;
		} else {
			printf("narrowColumnLocalSum should not arrive here\n");
		}
	}
}