#include "../predictor/include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void unPredict(unsigned long * inputResidual,
    unsigned long * sample,
    struct arguments * parameters,
    unsigned int * sampleRep,
    int * localsum, int ** diffVector, long ** weights, 
    long sMin, long sMax, long sMid, 
    int maximumError, int sampleDamping, int sampleOffset) {

    long long doubleResPredSample = 0;
    long long clippedBin = 0;

    for (int z = 0; z < parameters->zSize; z++)
    {
        for (int y = 0; y < parameters->ySize; y++)
        {
            for (int x = 0; x < parameters->xSize; x++)
            {
                
                if(x == 0 && y == 0) {
                    initWeights(weights, z, parameters);
                }

                wideNeighborLocalSum(sampleRep,localsum,x,y,z,parameters);
                BuildDiffVector(sampleRep,localsum,diffVector,x,y,z,parameters);

                long long highResSample = computeHighResPredSample(localsum, weights, diffVector, sMid, sMin, sMax, x, y, z, parameters);

                long long predictedSample = computePredictedSample(inputResidual, &doubleResPredSample, localsum, weights, diffVector, highResSample, sMid, sMin, sMax, x, y, z, parameters);

                long quantizerIndex = quantization(inputResidual, predictedSample, maximumError, x, y, z, parameters);
                
                sampleRep[offset(x,y,z, parameters)] = sampleRepresentation(inputResidual, &clippedBin, predictedSample, quantizerIndex, maximumError, highResSample, sampleDamping, sampleOffset, x, y, z, sMin, sMax, parameters);

                long long doubleResError = (2 * clippedBin) - doubleResPredSample; 
                updateWeightVector(weights, diffVector, doubleResError, x, y, z, parameters);

                if(parameters->debugMode != 0) {
                    printf("At X: %d, Y: %d, Z: %d, \n",x,y,z);
                    printVectors(diffVector, parameters);
                    printf("High resolution Sample is %lld \n", highResSample);
                    printf("predicted value is %lld \n", predictedSample);
                    printf("quantizer to sample is %ld \n", quantizerIndex);
                    printf("input residual is %u \n", inputResidual[offset(x,y,z, parameters)]);
                    printf("sample rep is %u \n", sampleRep[offset(x,y,z, parameters)]);
                    printf("Mapped residual: %lu \n", sample[offset(x,y,z,parameters)]);
                    printf("---------------------------\n");
                }

            }
            
        }
        
    }
    
}