#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void BuildDiffVector(uint32_t * sample,  int32_t * diffVector, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters, int32_t(*localSumFunc)(uint32_t *, uint16_t, uint16_t, uint16_t, struct arguments *)) {
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

int32_t centralLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if(x+y != 0) {
		return (sample[offset(x,y,z,parameters)] << 2) - localsum;
	} else {
		return 0;
	}
}

int32_t northLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if (y > 0) {
		return (4*sample[offset(x,y-1,z, parameters)]) - localsum;
	} else {
		return 0;
	}
}

int32_t westLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if ( x > 0 && y > 0) {
		return (4*sample[offset(x-1,y,z, parameters)]) - localsum;
	} else if (x == 0 && y > 0) {
		return (4*sample[offset(x,y-1,z, parameters)]) - localsum;
	} else {
		return 0;
	}
}

int32_t northwestLocalDiffrence(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, int32_t localsum, struct arguments * parameters) {
	if (x > 0 && y > 0) {
		return (4*sample[offset(x-1,y-1,z, parameters)]) - localsum;
	} else if (x == 0 && y > 0) {
		return (4*sample[offset(x,y-1,z, parameters)]) - localsum;
	} else {
		return 0;
	}
}