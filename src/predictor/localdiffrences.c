#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void BuildDiffVector(unsigned int * sample, int * localsum, long * diffVector, int x, int y, int z, struct arguments * parameters) {
	int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
	if (z > 0) {
		for (int i = 0; i < currentPredBands; i++) {
			diffVector[i] = centralLocalDiffrence(sample, localsum, x, y, z-i-1, parameters);
		}
	}
/* 	if (parameters->mode == FULL) {
		if(x+y != 0) {
			diffVector[parameters->precedingBands] = northLocalDiffrence(sample, localsum, x, y, z, parameters);
			diffVector[parameters->precedingBands+1] = westLocalDiffrence(sample, localsum, x, y, z, parameters);
			diffVector[parameters->precedingBands+2] = northwestLocalDiffrence(sample, localsum, x, y, z, parameters);
		}
	} */
}

int centralLocalDiffrence(unsigned int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if(x+y != 0) {
		return (sample[offset(x,y,z,parameters)] << 2) - localsum[offset(x,y,z,parameters)];
	} else {
		return 0;
	}
}


int northLocalDiffrence(unsigned int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if (y > 0) {
		return (4*sample[offset(x,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)];
	} else {
		return 0;
	}
}

int westLocalDiffrence(unsigned int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if ( x > 0 && y > 0) {
		return (4*sample[offset(x-1,y,z, parameters)]) - localsum[offset(x,y,z,parameters)];
	} else if (x == 0 && y > 0) {
		return (4*sample[offset(x,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)];
	} else {
		return 0;
	}
}

int northwestLocalDiffrence(unsigned int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if (x > 0 && y > 0) {
		return (4*sample[offset(x-1,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)];
	} else if (x == 0 && y > 0) {
		return (4*sample[offset(x,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)];
	} else {
		return 0;
	}
}