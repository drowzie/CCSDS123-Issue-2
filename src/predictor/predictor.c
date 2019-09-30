#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include <stdio.h>





int offset(int x, int y, int z, struct arguments * args) { 
    return (z * args->xSize * args->ySize) + (y * args->xSize) + x; 
}

void quantization(int * sample, int * predictedSample, int * fidelityControl, int * predictionResidual, int * quantizedSample, struct arguments * args) {
	for (int z = 0; z < args->zSize; z++) {
		for (int y = 0; y < args->ySize; y++) {
			for (int x = 0; x < args->xSize; x++) {
				if((x == 0 && y == 0)) {
					quantizedSample[offset(x,y,z, args)] = predictionResidual[offset(x,y,z, args)]; // prediction at 0
				} else {

				}
			}
		}
	}	
}