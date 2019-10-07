#include "include/localdiffrences.h"
#include "../utils/include/utilities.h"
#include "../../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void BuildDiffVector(int * sample, int * localsum, int ** diffVector, int x, int y, int z, struct arguments * parameters) {
	int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
	for (int i = 0; i < currentPredBands; i++) {
		diffVector[0][i] = centralLocalDiffrence(sample, localsum, x, y, z-i-1, parameters);
	}
	if (parameters->mode == FULL) {
		diffVector[1][0] = northLocalDiffrence(sample, localsum, x, y, z, parameters);
		diffVector[2][0] = westLocalDiffrence(sample, localsum, x, y, z, parameters);
		diffVector[3][0] = northwestLocalDiffrence(sample, localsum, x, y, z, parameters);
	}
}

int centralLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		return ((4 *  sample[offset(x,y,z,parameters)]) - localsum[offset(x,y,z,parameters)]);
	} else {
		return 0;
	}
}


int northLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		if (y > 0) {
			return ((4 * sample[offset(x,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)]);
		} else if (y == 0) {
			return 0;
		} else {
			printf("northLocalDiffrence should not arrive here\n");
			exit(EXIT_FAILURE);
		}
	} else	{
		return 0;
	}
}

int westLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		if ( x > 0 && y > 0) {
			return ((4 * sample[offset(x-1,y,z, parameters)]) - localsum[offset(x,y,z,parameters)]);
		} else if (x == 0 && y > 0) {
			return ((4 * sample[offset(x,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)]);
		} else if (y == 0) {
			return 0;
		} else {
			printf("westLocalDiffrence should not arrive here\n");
			exit(EXIT_FAILURE);
		}
	} else {
		return 0;
	}
}

int northwestLocalDiffrence(int * sample, int * localsum, int x, int y, int z, struct arguments * parameters) {
	if(!(x == 0 && y == 0)) {
		if (x > 0 && y > 0) {
			return ((4* sample[offset(x-1,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)]);
		} else if (x == 0 && y > 0) {
			return ((4* sample[offset(x,y-1,z, parameters)]) - localsum[offset(x,y,z,parameters)]);
		} else if (y == 0) {
			return 0;
		} else {
			printf("northwestLocalDiffrence should not arrive here\n");
			exit(EXIT_FAILURE);
		}
	} else {
		return 0;
	}
}