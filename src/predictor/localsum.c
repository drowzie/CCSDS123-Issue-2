#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

/*
	Chapter 4.4 in CCSDS 123 Issue 2
*/

void wideNeighborLocalSum(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters) {
	if (x+y != 0) {
		if (y == 0 && x > 0){
			localsum[offset(x,y,z,parameters)] = 4 * sample[offset(x-1,y,z, parameters)];
		} else if (y > 0 && x > 0 && x < parameters->xSize-1) {
			localsum[offset(x,y,z,parameters)] = sample[offset(x-1,y,z, parameters)] + sample[offset(x-1,y-1,z, parameters)] + sample[offset(x,y-1,z, parameters)] + sample[offset(x+1,y-1,z, parameters)];
		} else if (y > 0 && x == 0) {
			localsum[offset(x,y,z,parameters)] = 2 * (sample[offset(x+1,y-1,z, parameters)] + sample[offset(x,y-1,z, parameters)]);
		} else {
			localsum[offset(x,y,z,parameters)] = sample[offset(x-1,y,z, parameters)] + sample[offset(x-1,y-1,z, parameters)] + 2 * sample[offset(x,y-1,z, parameters)];
		}
	}
}

void narrowNeighborLocalSum(unsigned long * sample, long * localsum, int smid, int x, int y, int z, struct arguments * parameters) {
	if(x+y != 0) {
		if (y > 0 && x > 0 && x < parameters->xSize-1) {
			localsum[offset(x,y,z,parameters)] = sample[offset(x-1,y-1,z, parameters)] +  (2 * sample[offset(x,y-1,z, parameters)]) + sample[offset(x+1,y-1,z, parameters)];
		} else if (y == 0 && x > 0 && y > 0) {
			localsum[offset(x,y,z,parameters)] = 4 * sample[offset(x-1,y,z-1, parameters)];
		} else if (y > 0 && x == 0) {
			localsum[offset(x,y,z,parameters)] = 2 * (sample[offset(x,y-1,z, parameters)] + sample[offset(x+1,y-1,z, parameters)]);
		} else if (y > 0 && x == parameters->xSize - 1) {
			localsum[offset(x,y,z,parameters)] = 2 * (sample[offset(x-1,y-1,z, parameters)] + sample[offset(x,y-1,z, parameters)]);
		} else {
			localsum[offset(x,y,z,parameters)] = 4 * smid;
		}
	}
}

void wideColumnLocalSum(unsigned long * sample, long * localsum, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		if(y > 0) {
			localsum[offset(x,y,z,parameters)] = 4 * sample[offset(x,y-1,z, parameters)];
		} else if (y == 0 && x > 0) {
			localsum[offset(x,y,z,parameters)] = 4 * sample[offset(x-1,y,z, parameters)];
		} else {
			printf("wideColumnLocalSum should not arrive here\n");
		}
	}
}

void narrowColumnLocalSum(unsigned long * sample, long * localsum, int smid, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		if (y > 0) {
			localsum[offset(x,y,z,parameters)] = 4 * sample[offset(x,y-1,z, parameters)];
		} else if (y == 0 && x > 0 && z > 0) {
			localsum[offset(x,y,z,parameters)] = 4 * sample[offset(x-1,y,z-1, parameters)];
		} else if (y == 0 && x > 0 && z == 0) {
			localsum[offset(x,y,z,parameters)] = 4 * smid;
		} else {
			printf("narrowColumnLocalSum should not arrive here\n");
		}
	}
}
