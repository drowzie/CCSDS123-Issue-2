#include "include/predictor.h"
#include "cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void BuildDiffVector(uint32_t * sample,  int64_t * diffVector, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg, int64_t(*localSumFunc)(uint32_t *, uint16_t, uint16_t, uint16_t, struct arguments *)) {
	int currentPredBands = z < arg->precedingBands ? z : arg->precedingBands;
	if (z > 0) {
		for (int i = 0; i < currentPredBands; i++) {
			diffVector[i] = centralLocalDiffrence(sample, x, y, z-i-1, localSumFunc(sample, x, y, z-i-1, arg), arg);
		}
	}
	if (arg->mode == FULL) {
		if(x+y != 0) {
			diffVector[arg->precedingBands] = northLocalDiffrence(sample, x, y, z, localSumFunc(sample, x, y, z, arg), arg);
			diffVector[arg->precedingBands+1] = westLocalDiffrence(sample, x, y, z, localSumFunc(sample, x, y, z, arg), arg);
			diffVector[arg->precedingBands+2] = northwestLocalDiffrence(sample, x, y, z, localSumFunc(sample, x, y, z, arg), arg);
		}
	}
}

int64_t centralLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * arg) {
	if(x+y != 0) {
		return ((int64_t)4*sample[offset(x,y,z,arg)]) - localsum;
	} else {
		return 0;
	}
}

int64_t northLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * arg) {
	if (y > 0) {
		return ((int64_t)4*sample[offset(x,y-1,z, arg)]) - localsum;
	} else {
		return 0;
	}
}

int64_t westLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * arg) {
	if ( x > 0 && y > 0) {
		return ((int64_t)4*sample[offset(x-1,y,z, arg)]) - localsum;
	} else if (x == 0 && y > 0) {
		return ((int64_t)4*sample[offset(x,y-1,z, arg)]) - localsum;
	} else {
		return 0;
	}
}

int64_t northwestLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int64_t localsum, struct arguments * arg) {
	if (x > 0 && y > 0) {
		return ((int64_t)4*sample[offset(x-1,y-1,z, arg)]) - localsum;
	} else if (x == 0 && y > 0) {
		return ((int64_t)4*sample[offset(x,y-1,z, arg)]) - localsum;
	} else {
		return 0;
	}
}
