#include "predictor.h"
#include "cliparser/cli.h"
#include <stdio.h>
#include <stdlib.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

/* 
	Builds the difference vectors for the the P previous samples in the Z band. The localsum func is to allow a flexible way to change local sum functions in localsum.c
*/

void BuildDiffVector(uint16_t * sample,  int64_t * diffVector, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int32_t(*localSumFunc)(uint16_t *, uint16_t, uint16_t, uint16_t, struct arguments *)) {
	int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
	if (z > 0) {
		for (int i = 0; i < currentPredBands; i++) {
			diffVector[i] = centralLocalDiffrence(sample, x, y, z-i-1, localSumFunc(sample, x, y, z-i-1, parameters), parameters);
		}
	}
	if (parameters->mode == FULL) {
		if(x+y != 0) {
			diffVector[parameters->precedingBands] = northLocalDiffrence(sample, x, y, z, localSumFunc(sample, x, y, z, parameters), parameters);
			diffVector[parameters->precedingBands+1] = westLocalDiffrence(sample, x, y, z, localSumFunc(sample, x, y, z, parameters), parameters);
			diffVector[parameters->precedingBands+2] = northwestLocalDiffrence(sample, x, y, z, localSumFunc(sample, x, y, z, parameters), parameters);
		}
	}
}

int64_t centralLocalDiffrence(uint16_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if(x+y != 0) {
		return (int64_t)((4LL*sample[offset(x,y,z,parameters)]) - localsum);
	} else {
		return 0;
	}
}

int64_t northLocalDiffrence(uint16_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if (y > 0) {
		return (int64_t)((4*sample[offset(x,y-1,z, parameters)]) - localsum);
	} else {
		return 0;
	}
}

int64_t westLocalDiffrence(uint16_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if ( x > 0 && y > 0) {
		return (int64_t)((4*sample[offset(x-1,y,z, parameters)]) - localsum);
	} else if (x == 0 && y > 0) {
		return (int64_t)((4*sample[offset(x,y-1,z, parameters)]) - localsum);
	} else {
		return 0;
	}
}

int64_t northwestLocalDiffrence(uint16_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if (x > 0 && y > 0) {
		return (int64_t)((4*sample[offset(x-1,y-1,z, parameters)]) - localsum);
	} else if (x == 0 && y > 0) {
		return (int64_t)((4*sample[offset(x,y-1,z, parameters)]) - localsum);
	} else {
		return 0;
	}
}