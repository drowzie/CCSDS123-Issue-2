#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include <stdio.h>


int offset(int x, int y, int z) { 
    return (z * xSize * ySize) + (y * xSize) + x; 
}

/*
	Implementation of the different LOCALSUM variations.
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


/*
	Implementation of the directional local diffrences
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


void quantization(int * sample, int * predictedSample, int * fidelityControl, int * predictionResidual, int * quantizedSample) {
	for (int z = 0; z < zSize; z++) {
		for (int y = 0; y < ySize; y++) {
			for (int x = 0; x < xSize; x++) {
				if((x == 0 && y == 0)) {
					quantizedSample[offset(x,y,z)] = predictionResidual[offset(x,y,z)]; // prediction at 0
				} else {

				}
			}
		}
	}	
}