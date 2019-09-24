#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include <stdio.h>

/*
	Chapter 4.4 in CCSDS 123 Issue 2
*/

void wideNeighborLocalSum(int * sample, int * localsum) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if (y == 0 && x > 0){
						localsum[offset(x,y,z)] = 4 * sample[offset(x-1,y,z)];
					} else if (y > 0 && x > 0 && x < xSize-1) {
						localsum[offset(x,y,z)] = sample[offset(x-1,y,z)] + sample[offset(x-1,y-1,z)] + sample[offset(x,y-1,z)] + sample[offset(x+1,y-1,z)];
					} else if (y > 0 && x == 0) {
						localsum[offset(x,y,z)] = 2 * (sample[offset(x+1,y-1,z)] + sample[offset(x,y-1,z)]);
					} else if (y > 0 && x == xSize-1) {
						localsum[offset(x,y,z)] = sample[offset(x-1,y,z)] + sample[offset(x-1,y-1,z)] + 2 * sample[offset(x,y-1,z)];
					} else {
						printf("wideNeighborLocalSum should not arrive here\n");
					}
				}
			}
		}
	}
}

void narrowNeighborLocalSum(int * sample, int * localsum, int smid) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if (y > 0 && x > 0 && x < xSize-1)
					{
						localsum[offset(x,y,z)] = sample[offset(x-1,y-1,z)] +  (2 * sample[offset(x,y-1,z)]) + sample[offset(x+1,y-1,z)];
					} else if (y == 0 && x > 0 && y > 0) {
						localsum[offset(x,y,z)] = 4 * sample[offset(x-1,y,z-1)];
					} else if (y > 0 && x == 0) {
						localsum[offset(x,y,z)] = 2 * (sample[offset(x,y-1,z)] + sample[offset(x+1,y-1,z)]);
					} else if (y > 0 && x == xSize - 1) {
						localsum[offset(x,y,z)] = 2 * (sample[offset(x-1,y-1,z)] + sample[offset(x,y-1,z)]);
					} else if (y == 0 && x > 0 && z == 0) {
						localsum[offset(x,y,z)] = 4 * smid;
					} else {
						printf("narrowNeighborLocalSum should not arrive here\n");
					}
				}
			}
		}
	}
}

void wideColumnLocalSum(int * sample, int * localsum) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if(y > 0) {
						localsum[offset(x,y,z)] = 4 * sample[offset(x,y-1,z)];
					} else if (y == 0 && x > 0) {
						localsum[offset(x,y,z)] = 4 * sample[offset(x-1,y,z)];
					} else {
						printf("wideColumnLocalSum should not arrive here\n");
					}
				}
			}
		}
	}
}

void narrowColumnLocalSum(int * sample, int * localsum, int smid) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if(!(x == 0 && y == 0)) {
					if (y > 0)
					{
						localsum[offset(x,y,z)] = 4 * sample[offset(x,y-1,z)];
					} else if (y == 0 && x > 0 && z > 0) {
						localsum[offset(x,y,z)] = 4 * sample[offset(x-1,y,z-1)];
					} else if (y == 0 && x > 0 && z == 0) {
						localsum[offset(x,y,z)] = 4 * smid;
					} else {
						printf("narrowColumnLocalSum should not arrive here\n");
					}
				}
			}
		}
	}
}