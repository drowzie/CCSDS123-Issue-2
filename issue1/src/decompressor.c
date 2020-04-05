#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "cliparser/cli.h"
#include "predictor/predictor.h"
#include "encoder/encoder.h"
#include "utils/utilities.h"

int decompressImage(FILE * compressedImage, FILE * decompressedImageFile, struct arguments * parameters) { 
    readImageHeader(compressedImage, parameters);
    int error = 0;
    uint16_t * originalImage = malloc(parameters->xSize*parameters->ySize*parameters->zSize*sizeof(uint16_t));
    //Predictor
	int32_t * weights = malloc((parameters->mode != REDUCED ? parameters->precedingBands+3 : parameters->precedingBands) * sizeof(int32_t ));
    if (weights == NULL) {
        error = -1;
        goto free;
    }
    
	int32_t * diffVector = malloc((parameters->mode != REDUCED ? parameters->precedingBands+3 : parameters->precedingBands) * sizeof(int32_t ));
    if (diffVector == NULL) {
        error = -1;
        goto free;
    }
	// Encoder
    uint16_t * counter = malloc(sizeof(uint16_t)*parameters->zSize);
    if (counter == NULL) {
        error = -1;
        goto free;
    }
    uint64_t * accumulator = malloc(sizeof(uint64_t)*parameters->zSize);
    if(accumulator == NULL) {
        error = -1;
        goto free;
    }

	if (parameters->encodeOrder == BSQ) {
		for (uint16_t z = 0; z < parameters->zSize; z++) {
			for (uint16_t y = 0; y < parameters->ySize; y++) {
				for (uint16_t x = 0; x < parameters->xSize; x++) {
                    uint32_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, compressedImage, parameters);
                    originalImage[offset(x,y,z, parameters)] = unPredict(originalImage, tempResidual, x, y, z, parameters, diffVector, weights);
                    if(parameters->pixelType == SIGNED) {
                        int16_t signedSample = originalImage[offset(x,y,z, parameters)] - parameters->sMid;
                        fwrite(&signedSample, 2, 1, decompressedImageFile);
                    } else {
                        fwrite(&originalImage[offset(x,y,z, parameters)], 2, 1, decompressedImageFile);
                    }
				}
			}
		}
	} else if (parameters->encodeOrder == BIP) {
		for (uint16_t y = 0; y < parameters->ySize; y++) {
			for (uint16_t x = 0; x < parameters->xSize; x++) {
				for (uint16_t z = 0; z < parameters->zSize; z++) {
                    uint16_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, compressedImage, parameters);
                    originalImage[offset(x,y,z, parameters)] = unPredict(originalImage, tempResidual, x, y, z, parameters, diffVector, weights);
                    if(parameters->pixelType == SIGNED) {
                        int16_t signedSample = originalImage[offset(x,y,z, parameters)] - parameters->sMid;
                        fwrite(&signedSample, 2, 1, decompressedImageFile);
                    } else {
                        fwrite(&originalImage[offset(x,y,z, parameters)], 2, 1, decompressedImageFile);
                    }
				}
			}
		}
	} else if(parameters->encodeOrder == BIL) {
		for (uint16_t y = 0; y < parameters->ySize; y++) {
			for (uint16_t x = 0; x < parameters->zSize; x++) {
				for (uint16_t z = 0; z < parameters->xSize; z++) {
                    uint16_t tempResidual = decodeSampleAdaptive(counter, accumulator, x, y, z, compressedImage, parameters);
                    originalImage[offset(x,y,z, parameters)] = unPredict(originalImage, tempResidual, x, y, z, parameters, diffVector, weights);
                    if(parameters->pixelType == SIGNED) {
                        int16_t signedSample = originalImage[offset(x,y,z, parameters)] - parameters->sMid;
                        fwrite(&signedSample, 2, 1, decompressedImageFile);
                    } else {
                        fwrite(&originalImage[offset(x,y,z, parameters)], 2, 1, decompressedImageFile);
                    }
				}
			}
		}
    }
free: 
    free(weights);
    free(diffVector);
    free(counter);
    free(accumulator);
    return error;
}