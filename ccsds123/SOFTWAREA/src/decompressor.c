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
    printf("x %d y %d z %d \n", parameters->xSize, parameters->ySize, parameters->zSize);
    uint16_t * originalImage = malloc(parameters->xSize*parameters->ySize*parameters->zSize*sizeof(uint16_t));
    //Predictor
	int32_t ** weights = malloc(parameters->zSize*sizeof(int32_t *));
    if (weights == NULL) {
        return 1;
    }
	for (int i = 0; i < parameters->zSize; i++) {
		weights[i] = calloc((parameters->mode != REDUCED ? parameters->precedingBands+3 : parameters->precedingBands), sizeof(int32_t ));
	}
	int64_t * diffVector = calloc((parameters->mode != REDUCED ? parameters->precedingBands+3 : parameters->precedingBands), sizeof(int64_t ));
    if (diffVector == NULL) {
        error = -1;
        goto free;
    }
	// Encoder
	// Encoder
    uint16_t * counter = calloc(parameters->zSize, sizeof(uint16_t));
    if (counter == NULL) {
        return 1;
    }
    uint64_t * accumulator = calloc(parameters->zSize, sizeof(uint64_t));
    if(accumulator == NULL) {
        return 1;
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
			for (uint16_t z = 0; z < parameters->zSize; z++) {
				for (uint16_t x = 0; x < parameters->xSize; x++) {
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
