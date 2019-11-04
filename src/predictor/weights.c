#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

/*
    Initial weights for each Z band.
    Calloc defaults 0 for directional
*/

void initWeights(long ** weights, int z, struct arguments * parameters) {
    if(parameters->precedingBands > 0){
        weights[CENTRAL][0] = 7 << (parameters->weightResolution - 3);
        for(int i = 1; i < parameters->precedingBands; i++){
            weights[CENTRAL][i] = weights[CENTRAL][i - 1] >> 3;
        }
    }
    if(parameters->mode == FULL) {
        for(int i = 1; i < 4; i++) {
            weights[i][0] = 0;
        }
    }
    
}

/* 
    Update_weights will update weights according to chapter 4.10 in Issue 2
 */
void updateWeightVector(long ** weights, int ** diffVector, long long error, int x, int y, int z, struct arguments * parameters) {
    
    int signError = error < 0 ? -1 : 1;
    int scalingExp = parameters->weightMin + (int)((((y*parameters->xSize + x)-parameters->xSize))/parameters->weightInterval);
    scalingExp = clip(scalingExp, parameters->weightMin, parameters->weightMax);
    scalingExp += parameters->dynamicRange - parameters->weightResolution;
    //TODO: Add sigma for scaling exp: Note 2 diffrent sigma for directional and central diffrences
    
    if(z > 0) {
        int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
        for(int i = 0; i < currentPredBands; i++) { 
            if(scalingExp > 0) {
                weights[CENTRAL][i] = weights[CENTRAL][i] + ((((signError * diffVector[CENTRAL][z-i-1]) >> scalingExp) + 1) >> 1);
            } else {
                weights[CENTRAL][i] = weights[CENTRAL][i] + ((((signError * diffVector[CENTRAL][z-i-1]) << (-1*scalingExp)) + 1) >> 1);
            }
            int weightLimit = 1 << (parameters->weightResolution + 2);
            weights[CENTRAL][i] = clip(weights[CENTRAL][i], (-1 * weightLimit), (weightLimit -1));
        }
    }

    if(parameters->mode == FULL) {
        for (int i = 1; i < 4; i++) {
            if(scalingExp > 0) {
                weights[i][0] = weights[i][0] + ((((signError * diffVector[i][0]) >> scalingExp) + 1) >> 1);
            } else {
                weights[i][0] = weights[i][0] + ((((signError * diffVector[i][0]) << (-1*scalingExp)) + 1) >> 1);
            }
            int weightLimit = 0x1 << (parameters->weightResolution + 2);
            weights[i][0] = clip(weights[i][0], (-1 * weightLimit), (weightLimit -1));
        }
    }
}