#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

/*
    Initial weights for each Z band.
    Calloc defaults 0 for directional
*/

void initWeights(int32_t * weights, uint16_t z, struct arguments * parameters) {
    if(parameters->precedingBands > 0){
        weights[0] = 7 << (parameters->weightResolution - 3);
        int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;
        for(int i = 1; i < currentPredictionBand; i++){
            weights[i] = weights[i - 1] >> 3;
        }
    }
    if(parameters->mode == FULL) {
        for(int i = 0; i < 3; i++) {
            weights[parameters->precedingBands + i] = 0;
        }
    }
    
}

/* 
    Update_weights will update weights according to chapter 4.10 in Issue 2
 */
void updateWeightVector(int32_t * weights, int32_t * diffVector, int64_t error, uint16_t x, uint16_t y, uint16_t z, int interbandOffset, int intrabandExponent, struct arguments * parameters) {
    
    long weightLimit = 0x1 << (parameters->weightResolution + 2);
    int signError = error < 0 ? -1 : 1;
    long scalingExp = 0;
    scalingExp = ((y-1)*parameters->xSize+x) >> parameters->weightInterval;
    scalingExp = clip(parameters->weightMin + scalingExp, parameters->weightMin, parameters->weightMax);
    scalingExp += parameters->dynamicRange - parameters->weightResolution;
    int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;

    long tmp = 0;
    for(int i = 0; i < currentPredictionBand; i++) { 
        if((scalingExp+interbandOffset) > 0) {
            tmp = (signError * diffVector[i]) >> (scalingExp+interbandOffset);
        } else {
            tmp = (signError * diffVector[i]) << (-1*(scalingExp+interbandOffset));
        }
        tmp = (tmp + 1) >> 1;
        weights[i] = clip(weights[i] + tmp, (-1 * weightLimit), (weightLimit - 1));
    }

    if(parameters->mode == FULL) {
        for (int i = 0; i < 3; i++) {
            if((scalingExp+intrabandExponent) > 0) {
                tmp = (signError * diffVector[parameters->precedingBands + i]) >> (scalingExp+intrabandExponent);
            } else {
                tmp = (signError * diffVector[parameters->precedingBands + i]) << (-1*(scalingExp+intrabandExponent));
            }
            tmp = (tmp + 1) >> 1;
            weights[parameters->precedingBands + i] = clip(weights[parameters->precedingBands + i] + tmp, (-1 * weightLimit), (weightLimit-1));
        }
    }
}