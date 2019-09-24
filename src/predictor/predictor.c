#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include <stdio.h>


int offset(int x, int y, int z) { 
    return (z * xSize * ySize) + (y * xSize) + x; 
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