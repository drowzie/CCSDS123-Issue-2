#include "include/predictor.h"
#include "../cliparser/include/cli.h"

/*
    Initial weights for each Z band.
    Calloc defaults 0 for directional
*/

void initWeights(long ** weights, int z, struct arguments * parameters) {
    if(parameters->precedingBands > 0){
        weights[0][0] = 7 << (parameters->weightResolution - 3);
        for(int i = 1; i < parameters->precedingBands; i++){
            weights[0][i] = weights[0][i - 1] >> 3;
        }
    }
}
/* 
    Update_weights will update weights according to chapter 4.10 in Issue 2
 */
void updateWeightVector(long ** weights, int ** diffVector, long long error, int x, int y, int z, struct arguments * parameters) {
    
    int signError = sgnplus(error);
    int scalingExp = parameters->weightMin + ((int)(y*parameters->xSize + x - parameters->xSize))/parameters->weightInterval;
    scalingExp = clip(scalingExp, parameters->weightMin, parameters->weightMax);
    scalingExp += parameters->dynamicRange - parameters->weightResolution;
    //TODO: Add sigma for scaling exp: Note 2 diffrent sigma for directional and central diffrences

    if(z > 0) {
        int currentPredBands = z < parameters->precedingBands ? z : parameters->precedingBands;
        for(int i = 0; i < currentPredBands; i++) { 
            if(scalingExp > 0) {
                weights[0][i] = weights[0][i] + (((signError * diffVector[0][z-i-1] >> scalingExp) + 1) >> 1);
            } else {
                weights[0][i] = weights[0][i] + (((signError * diffVector[0][z-i-1] << -1*scalingExp) + 1) >> 1);
            }
            int weightLimit = 0x1 << (parameters->weightResolution + 2);
            weights[0][i] = (long) clip(weights[0][i], (-1 * weightLimit), (weightLimit -1));
        }

        if(parameters->mode == FULL) {
            for (int i = 1; i < 4; i++) {
                if(scalingExp > 0) {
                    weights[i][0] = weights[i][0] + (((signError * diffVector[i][0] >> scalingExp) + 1) >> 1);
                } else {
                    weights[i][0] = weights[i][0] + (((signError * diffVector[i][0] << -1*scalingExp) + 1) >> 1);
                }
                int weightLimit = 0x1 << (parameters->weightResolution + 2);
                weights[i][0] = (long) clip(weights[i][0], (-1 * weightLimit), (weightLimit -1));
            }
        }
    }
}