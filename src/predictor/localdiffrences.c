#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void BuildDiffVector(int * sample, int * localsum, int ** diffVector, int x, int y, int z, struct arguments * parameters) {
	int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
	for (int i = 0; i < currentPredBands; i++) {
		diffVector[CENTRAL][i] = centralLocalDiffrence(sample, localsum, x, y, z-i-1, parameters);
	}
	if (parameters->mode == FULL) {
		diffVector[NORTH][0] = northLocalDiffrence(sample, localsum, x, y, z, parameters);
		diffVector[WEST][0] = westLocalDiffrence(sample, localsum, x, y, z, parameters);
		diffVector[NORTHWEST][0] = northwestLocalDiffrence(sample, localsum, x, y, z, parameters);
	}
}

int centralLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		return (((sample[offset(x,y,z,parameters)])<<2) - localsum[offset(x,y,z,parameters)]);
	} else {
		return 0;
	}
}


int northLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if (y > 0) {
		return (((sample[offset(x,y-1,z, parameters)])<<2) - localsum[offset(x,y,z,parameters)]);
	} else if (y == 0 || (x == 0 && y == 0)) {
		return 0;
	} else {
		printf("northLocalDiffrence should not arrive here\n");
		exit(EXIT_FAILURE);
	}
}

int westLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if ( x > 0 && y > 0) {
		return (( (sample[offset(x-1,y,z, parameters)]) << 2) - localsum[offset(x,y,z,parameters)]);
	} else if (x == 0 && y > 0) {
		return (((sample[offset(x,y-1,z, parameters)]) << 2) - localsum[offset(x,y,z,parameters)]);
	} else if (y == 0 || (x == 0 && y == 0)) {
		return 0;
	} else {
		printf("westLocalDiffrence should not arrive here\n");
		exit(EXIT_FAILURE);
	}
}

int northwestLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if (x > 0 && y > 0) {
		return (((sample[offset(x-1,y-1,z, parameters)])<<2) - localsum[offset(x,y,z,parameters)]);
	} else if (x == 0 && y > 0) {
		return (((sample[offset(x,y-1,z, parameters)])<<2) - localsum[offset(x,y,z,parameters)]);
	} else if (y == 0|| (x == 0 && y == 0)) {
		return 0;
	} else {
		printf("northwestLocalDiffrence should not arrive here\n");
		exit(EXIT_FAILURE);
	}
}