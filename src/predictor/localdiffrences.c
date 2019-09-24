#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include <stdio.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void centralLocalDiffrence(int * sample, int * localsum, int * centraldiffrence) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					centraldiffrence[offset(x,y,z)] = ((4 *  sample[offset(x,y,z)]) - localsum[offset(x,y,z)]);
				}
			}
		}
	}
}


void northLocalDiffrence(int * sample, int * localsum, int * northLocalDiffrence) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if (y > 0) {
						northLocalDiffrence[offset(x,y,z)] = ((4 * sample[offset(x,y-1,z)]) - localsum[offset(x,y,z)]);
					} else if (y == 0) {
						northLocalDiffrence[offset(x,y,z)] = 0;
					} else {
						printf("northLocalDiffrence should not arrive here\n");
					}
				}
			}
		}
	}
}

void westLocalDiffrence(int * sample, int * localsum, int * westLocalDiffrence) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if ( x > 0 && y > 0) {
						westLocalDiffrence[offset(x,y,z)] = ((4 * sample[offset(x-1,y,z)]) - localsum[offset(x,y,z)]);
					} else if (x == 0 && y > 0) {
						westLocalDiffrence[offset(x,y,z)] = ((4 * sample[offset(x,y-1,z)]) - localsum[offset(x,y,z)]);
					} else if (y == 0) {
						westLocalDiffrence[offset(x,y,z)] = 0;
					} else {
						printf("westLocalDiffrence should not arrive here\n");
					}
				}
			}
		}
	}
}

void northwestLocalDiffrence(int * sample, int * localsum, int * northwestLocalDiffrence) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if (x > 0 && y > 0) {
						northwestLocalDiffrence[offset(x,y,z)] = ((4* sample[offset(x-1,y-1,z)]) - localsum[offset(x,y,z)]);
					} else if (x == 0 && y > 0) {
						northwestLocalDiffrence[offset(x,y,z)] = ((4* sample[offset(x,y-1,z)]) - localsum[offset(x,y,z)]);
					} else if (y == 0) {
						northwestLocalDiffrence[offset(x,y,z)] = 0;
					} else {
						printf("northwestLocalDiffrence should not arrive here\n");
					}
				}
			}
		}
	}
}