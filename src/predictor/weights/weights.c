#include "include/weights.h"
#include "../../cliparser/include/cli.h"
#include "../utils/include/utilities.h"

/*
    Startup weights for each Z band. 
*/

void init_weights(int * weights, int z, struct arguments * parameters) {

    if(parameters->precedingBands > 0){
        weights[0] = 7 << (parameters->weightResolution - 3);
        for(int i = 1; i < parameters->precedingBands; i++){
            weights[i] = weights[i - 1] >> 3;
        }   
    }

    if(parameters->mode != REDUCED){
        for(int i = 0; i < 3; i++){
            weights[parameters->precedingBands + i] = 0;
        }
    }
}
/* 
    Update_weights will update weights according to chapter 4.10 in Issue 2
 */
void update_weights(int ** weights, int ** localDiffrences,int error, int x, int y, int z, struct arguments * parameters) {
    

    int signError = sgnplus(error);
    int scalingExp = parameters->weightMin + ((int)(y*parameters->xSize + x - parameters->xSize))/parameters->weightInterval;
    scalingExp = clip(scalingExp, parameters->weightMin, parameters->weightMax);
    scalingExp += parameters->dynamicRange - parameters->weightResolution;

    if(z > 0) {
        int cur_pred_bands = z < parameters->precedingBands ? z : parameters->precedingBands;
        for(int i = 0; i < cur_pred_bands; i++) { 
            if(scalingExp > 0) {
                // Central
                weights[i][0] = weights[i][0] + (((signError * localDiffrences[0][z-i-1] >> scalingExp) + 1) >> 1);
            } else {
                weights[i][0] = weights[i][0] + (((signError * localDiffrences[0][z-i-1] << -1*scalingExp) + 1) >> 1);
            }
            int weightLimit = 0x1 << (parameters->weightResolution + 2);
            weights[i] = clip(weights[i], (-1 * weightLimit), (weightLimit -1));

            if(parameters->mode == FULL) {
                if(scalingExp > 0) {
                    
                } else {
                    
                }
            }
        }
    }
}