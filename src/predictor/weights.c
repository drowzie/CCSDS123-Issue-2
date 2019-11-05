#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

/*
    Initial weights for each Z band.
    Calloc defaults 0 for directional
*/

void initWeights(long * weights, int z, struct arguments * parameters) {
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
void updateWeightVector(long * weights, long * diffVector, long long error, int x, int y, int z, struct arguments * parameters) {
    
    long weightLimit = 0x1 << (parameters->weightResolution + 2);
    int signError = error < 0 ? -1 : 1;

    long scalingExp = ((y-1*parameters->xSize)+x) >> parameters->weightInterval;
    scalingExp = clip(parameters->weightMin + scalingExp, parameters->weightMin, parameters->weightMax);
    scalingExp += parameters->dynamicRange - parameters->weightResolution;
    //TODO: Add sigma for scaling exp: Note 2 diffrent sigma for directional and central diffrences
    if(z > 0) {
        int currentPredictionBand = z < parameters->precedingBands ? z : parameters->precedingBands;
        for(int i = 0; i < currentPredictionBand; i++) { 
            if(scalingExp > 0) {
                weights[i] += (signError * diffVector[i]) >> scalingExp;
            } else {
                weights[i] += (signError * diffVector[i]) << (-1*scalingExp);
            }
            weights[i] = (weights[i] + 1) >> 1;
            weights[i] = clip(weights[i], (-1 * weightLimit), (weightLimit - 1));
        }
    }

/*     if(parameters->mode == FULL) {
        for (int i = 0; i < 3; i++) {
            if(scalingExp > 0) {
                weights[parameters->precedingBands + i] += (signError * diffVector[parameters->precedingBands + i]) >> scalingExp;
            } else {
                weights[parameters->precedingBands + i] += (signError * diffVector[parameters->precedingBands + i]) << (-1*scalingExp);
            }
            weights[parameters->precedingBands + i] = (weights[parameters->precedingBands + i] + 1) >> 1;
            weights[parameters->precedingBands + i] = clip(weights[parameters->precedingBands + i], (-1 * weightLimit), (weightLimit-1));
        }
    } */
}