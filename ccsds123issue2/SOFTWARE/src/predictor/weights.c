#include "include/predictor.h"
#include "cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
    Initial weights for each Z band.
    Calloc defaults 0 for directional
*/

void initWeights(int64_t ** weights, uint16_t z, struct arguments * arg) {
    if(arg->precedingBands > 0) {
        weights[z][0] = 7LL << (arg->weightResolution - 3);
        int currentPredictionBand = z < arg->precedingBands ? z : arg->precedingBands;
        for(int i = 1; i < currentPredictionBand; i++) {
            weights[z][i] = weights[z][i - 1] >> 3;
        }
    }
    if(arg->mode == FULL) {
        for(int i = 0; i < 3; i++) {
            weights[z][arg->precedingBands + i] = 0;
        }
    }
}

/*
    Update_weights will update weights according to chapter 4.10 in Issue 2
 */
void updateWeightVector(int64_t ** weights, int64_t * diffVector, int64_t error, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {

    int64_t weightLimit = 1LL << (arg->weightResolution + 2);
    int signError = error < 0 ? -1 : 1;
    int64_t scalingExp = 0;
    scalingExp = (((int64_t)y-1LL)*(int64_t)arg->xSize+(int64_t)x) >> arg->weightInterval;
    scalingExp = clip(arg->weightMin + scalingExp, arg->weightMin, arg->weightMax);
    scalingExp += (int64_t)arg->dynamicRange - (int64_t)arg->weightResolution;
    int currentPredictionBand = z < arg->precedingBands ? z : arg->precedingBands;

    int64_t tmp = 0;
    for(int i = 0; i < currentPredictionBand; i++) {
        if((scalingExp) > 0) {
            tmp = signError * diffVector[i] >> (scalingExp);
        } else {
            tmp = (signError * diffVector[i]) << (-1*((scalingExp)));
        }
        tmp = (tmp + 1) >> 1;
        weights[z][i] = clip(weights[z][i] + tmp, (-1 * weightLimit), (weightLimit - 1));
    }

    if(arg->mode == FULL) {
        for (int i = 0; i < 3; i++) {
            if((scalingExp+arg->intrabandExponent) > 0) {
                tmp = (signError * diffVector[arg->precedingBands + i]) >> (scalingExp);
            } else {
                tmp = (signError * diffVector[arg->precedingBands + i]) << (llabs(scalingExp));
            }
            tmp = (tmp + 1) >> 1;
            weights[z][arg->precedingBands + i] = clip(weights[z][arg->precedingBands + i] + tmp, (-1 * weightLimit), (weightLimit-1));
        }
    }
}
