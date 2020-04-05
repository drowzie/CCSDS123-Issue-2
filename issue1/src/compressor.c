
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "cliparser/cli.h"
#include "predictor/predictor.h"
#include "encoder/encoder.h"
#include "utils/utilities.h"

/* 
    Compresses the image by the CCSDS 123 Issue 1 standard.
    The compressed image pointer is the destination of the compression algorithm where the total amount of bytes writtes is located in totalWrittenBytes
 */
int compressImage(uint16_t * image, uint8_t * compressedImage, unsigned int * totalWrittenBytes, struct arguments * parameters) {
	//Predictor
	int32_t * weights = malloc((parameters->mode != REDUCED ? parameters->precedingBands+3 : parameters->precedingBands) * sizeof(int32_t ));
    if (weights == NULL) {
        return 1;
    }
    
	int32_t * diffVector = malloc((parameters->mode != REDUCED ? parameters->precedingBands+3 : parameters->precedingBands) * sizeof(int32_t ));
    if (diffVector == NULL) {
        return 1;
    }
	// Encoder
    uint16_t * counter = malloc(sizeof(uint16_t)*parameters->zSize);
    if (counter == NULL) {
        return 1;
    }
    uint64_t * accumulator = malloc(sizeof(uint64_t)*parameters->zSize);
    if(accumulator == NULL) {
        return 1;
    }
	unsigned int numWrittenBits = 0;

	// Add the image header
	writeImageHeader(&numWrittenBits, totalWrittenBytes, compressedImage, parameters);
	if (parameters->encodeOrder == BSQ) {
		for (uint16_t z = 0; z < parameters->zSize; z++) {
			for (uint16_t y = 0; y < parameters->ySize; y++) {
				for (uint16_t x = 0; x < parameters->xSize; x++) {
					uint16_t residuals = predict(image, x, y, z, parameters, diffVector, weights);
					encodeSampleAdaptive(residuals, counter, accumulator, x, y, z, totalWrittenBytes, &numWrittenBits, compressedImage, parameters);
				}
			}
		}
	} else if (parameters->encodeOrder == BIP) {
		for (uint16_t y = 0; y < parameters->ySize; y++) {
			for (uint16_t x = 0; x < parameters->xSize; x++) {
				for (uint16_t z = 0; z < parameters->zSize; z++) {
					uint32_t residuals = predict(image, x, y, z, parameters, diffVector, weights);
					encodeSampleAdaptive(residuals, counter, accumulator, x, y, z, totalWrittenBytes, &numWrittenBits, compressedImage, parameters);
				}
			}
		}
	} else if(parameters->encodeOrder == BIL) {
		for (uint16_t y = 0; y < parameters->ySize; y++) {
			for (uint16_t x = 0; x < parameters->zSize; x++) {
				for (uint16_t z = 0; z < parameters->xSize; z++) {
					uint32_t residuals = predict(image, x, y, z, parameters, diffVector, weights);
					encodeSampleAdaptive(residuals, counter, accumulator, x, y, z, totalWrittenBytes, &numWrittenBits, compressedImage, parameters);
				}
			}
		}
	} else {
		printf("Illeagal encoding order \n");
		exit(1);
	}
	/* 
		Fill the final bits to reach the word size of the computer
	*/
	fillBits(&numWrittenBits, totalWrittenBytes, compressedImage, parameters);
	
	// Free up stuff
	free(accumulator);
	free(counter);
 	free(weights);
	free(diffVector);

	return 0;
}
