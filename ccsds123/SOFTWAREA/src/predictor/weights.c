#include "predictor.h"
#include "cliparser/cli.h"
#include <stdio.h>
#include <stdlib.h>
/*
    Initial weights for each Z band.
    Calloc defaults 0 for directional
*/

void initWeights(int32_t ** weights, uint16_t z, struct arguments * parameters) {
    if(parameters->precedingBands > 0){
        weights[z][0] = 7L << (parameters->weightResolution - 3);
        int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;
        for(int i = 1; i < currentPredictionBand; i++){
            weights[z][i] = weights[z][i - 1] >> 3;
        }
    }
    if(parameters->mode == FULL) {
        for(int i = 0; i < 3; i++) {
            weights[z][parameters->precedingBands + i] = 0;
        }
    }
}

/* 
    Update_weights will update weights according to chapter 4.10 in Issue 2
*/
void updateWeightVector(int32_t ** weights, int64_t * diffVector, int32_t error, uint16_t x, uint16_t y, uint16_t z, struct arguments * parameters) {
    
    int64_t weightLimit = 0x1LL << (parameters->weightResolution + 2);
    int signError = error < 0 ? -1 : 1;
    int64_t scalingExp = 0;
    scalingExp = ((y-1)*parameters->xSize+x) >> parameters->weightInterval;
    scalingExp = clip(parameters->weightMin + scalingExp, parameters->weightMin, parameters->weightMax);
    scalingExp += parameters->dynamicRange - parameters->weightResolution;
    int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;

    int64_t tmp = 0;
    for(int i = 0; i < currentPredictionBand; i++) { 
        if((scalingExp) > 0) {
            tmp = (signError* diffVector[i]) >> (scalingExp);
        } else {
            tmp = (signError * diffVector[i]) << (-1*(scalingExp));
        }
        tmp = (tmp + 1) >> 1;
        weights[z][i] = clip(weights[z][i] + tmp, (-1 * weightLimit), (weightLimit - 1));
    }
    if(parameters->mode == FULL) {
        for (int i = 0; i < 3; i++) {
            if((scalingExp) > 0) {
                tmp = (signError * diffVector[parameters->precedingBands + i]) >> (scalingExp);
            } else {
                tmp = (signError * diffVector[parameters->precedingBands + i]) << (llabs(scalingExp));
            }
            tmp = (tmp + 1) >> 1;
            weights[z][parameters->precedingBands + i] = clip(weights[z][parameters->precedingBands + i] + tmp, (-1 * weightLimit), (weightLimit-1));
        }
    }
}