#include "include/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>
#include <math.h>

const char *argp_program_version = "0.0";
const char *argp_program_bug_address = "<chrisboltern3@gmail.com>";
static char doc[] = "CCSDS 123 Issue 2";
static char args_doc[] = "";

static struct argp_option options[] = {
    { "Full Prediction Mode", 'f', 0, OPTION_ARG_OPTIONAL, "Calculate in Full Prediction Mode (DEFAULT=REDUCED)", 0},
    { "debug", 777, 0, 0, "DEBUG MODE", 0},
    { "DECOMPRESS", 778, 0, 0, "DECOMPRESSION MODE. Input image is the compressed image", 0},
    { "BIGENDIAN", 779, 0 , 0, "Sample is BE, convert to LE", 0},
    { "SIGNED", 788, 0, 0, "SIGNED PIXELS", 0},
    { "PixelSize", 783,  "PIXELSIZE", 0 , "Pixel size of uncompressed image. 8 bits, 16 bits or 32 bits.", 0},
    { "imageorder", 'o',  "ORDER", 0 , "0=BSQ, 1=BIP or 2=BIL", 0},
    { "imageorder", 'O',  "ENCODEORDER", 0 , "0=BSQ, 1=BIP or 2=BIL", 0},
    { "Dynamic Range", 'd', "DYNRANGE", 0, "Register D size, #2-16", 0},
    { "Register size", 'r', "REGSIZE", 0, "Register R size, max{32, D +  Ω +  2} ≤ R ≤ 64", 0},
    { "Sample resolution", 's', "SRES", 0, "Sample Resolution(Θ), #0-4", 0},
	{ "Prediction Bands", 'p', "PBANDS", 0, "How many preceding spectral bands to use. #0-16", 0},
    { "Weight resolution", 'w', "WRES", 0, "Weight Resolution(Omega), #4-19", 0},
    { "Weight interval", 't', "Winterval", 0, "Chapter 4.8.2, CCSDS Issue 2", 0},
    { "Vmin", 'v', "vMin", 0, "Chapter 4.8.2, CCSDS Issue 2", 0},
    { "Vmax", 'V', "vMax", 0, "Chapter 4.8.2, CCSDS Issue 2", 0},
    { "xSize", 'x', "xSIZE", 0, "x size of image", 0},
    { "ySize", 'y', "ySIZE", 0, "y size of image", 0},
    { "zSize", 'z', "zSIZE", 0, "z size of image", 0},
    { "inputFILE", 'i', "FILE", 0, "FILENAME", 0},
    { "Theta", 'T', "theta", 0, "Lossy compression parameter", 0},
    { "FidelityControl", 'C', "fidelityControl", 0, "Method of fidelity control: 0=lossless, 1=absolute, 2=relative, 3=absolute and relative", 0},
    { "MaximumError", 'm', "error", 0, "Lossy compression parameter", 0},
    { "RelativeError", 'R', "Relerror", 0, "Lossy compression parameter", 0},
    { "sampleOffset", 'S', "sampleOffset", 0, "Lossy compression parameter", 0},
    { "SampleDamping", 'D', "sampleOffset", 0, "Lossy compression parameter", 0},
    { 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'd': arguments->dynamicRange = atoi(arg); break;
    case 'o':
      arguments->imageOrder = atoi(arg);
        break;
    case 'O':
        arguments->encodeOrder = atoi(arg);
        break;
    case 'i': arguments->fileName = arg; break;
    case 777: arguments->debugMode = 1; break;
    case 778: arguments->compressionMode = DECOMPRESS; break;
    case 779: arguments->endianess = BE; break;
    case 788: arguments->pixelType = SIGNED; break;
    case 783: arguments->pixelSize = atoi(arg); break;
    case 'r': arguments->registerSize = atoi(arg); break;
    case 'f': arguments->mode = FULL; break;
    case 'p': arguments->precedingBands = atoi(arg) < 0 ? 0 : atoi(arg) > 15 ? 15 : atoi(arg)  ; break;
    case 'x': arguments->xSize = atoi(arg); break;
    case 'y': arguments->ySize = atoi(arg); break;
    case 'z': arguments->zSize = atoi(arg); break;
    case 'T': arguments->theta = atol(arg); break;
    case 'C': arguments->fidelityControl = atoi(arg); break;
    case 'm': arguments->maximumError = atol(arg); break;
    case 'R': arguments->relativeError = atol(arg); break;
    case 'S': arguments->sampleOffset = atol(arg); break;
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

/*
    This function will init all values to a default value
 */

void parseArguments(int argc, char **argv, struct arguments * arguments) {
    arguments->compressionMode = COMPRESS;
    // Image sizes, defaults to test pattern
    arguments->xSize = 50;
    arguments->ySize = 50;
    arguments->zSize = 50;
    arguments->endianess = LE;
    arguments->pixelSize=0;
    // Prediction specific
    arguments->mode = REDUCED;
    arguments->dynamicRange = 16;
    arguments->precedingBands = 0;
    arguments->weightResolution = 4;
    arguments->weightMin = -5;
    arguments->weightMax = -4;
    arguments->weightInterval = 4;
    arguments->debugMode = 0;
    arguments->sumType = 2;
    arguments->registerSize = 64;
    // Lossy copmression parameters
    arguments->theta = 0;
    arguments->fidelityControl = ABSOLUTE;
    arguments->maximumError = 1<<10;
    arguments->relativeError = 0;
    arguments->interbandOffset = 0;
    arguments->sampleOffset = 0;
    arguments->sampleDamping = 0;
    arguments->intrabandExponent = 0;
    // Encoder specifics
    arguments->uMax = 14;
    arguments->initialK = 0;
    arguments->initialY = 7;
    arguments->yStar = 9;

    arguments->wordSize = 8;
    arguments->pixelType = UNSIGNED;
    arguments->imageOrder = BSQ;
    arguments->encodeOrder = BSQ;
    // Hybrid Encoder
    arguments->initialAccumulator = 0;
    argp_parse(&argp, argc, argv, 0, 0, arguments);
    arguments->sMin = 0;
    arguments->sMax = (1ULL<<arguments->dynamicRange)-1;
    arguments->sMid = 1ULL<<(arguments->dynamicRange-1);

    if (arguments->pixelSize != 8 && arguments->pixelSize != 16 && arguments->pixelSize != 32 && arguments->compressionMode == COMPRESS) {
        printf("Reading wrong pixel size %d \n", arguments->pixelSize);
        exit(EXIT_FAILURE);
    }

}



void printParameters(struct arguments * parameters) {
    printf("#################################################################### \n");
    printf("####################IMAGE PARAMETERS################################ \n");
    printf("Computing X: %d, Y: %d, Z: %d \n", parameters->xSize, parameters->ySize, parameters->zSize);
    printf("Image encoding order: %d , Reading image as %d (0=BSQ, 1=BIP, 2=BIL)\n", parameters->encodeOrder, parameters->imageOrder );
    printf("####################PREDICTOR PARAMETERS############################ \n");
	printf("Smax %lu, Smid %lu, smin %lu \n", parameters->sMax, parameters->sMid, parameters->sMin);
	printf("Dyn range %d Register size is %u \n", parameters->dynamicRange, parameters->registerSize);
    printf("WEIGHT RESOLUTION: %d \n", parameters->weightResolution);
    printf("Weight min: %d, Weight max: %d \n", parameters->weightMin, parameters->weightMax);
    printf("Weight interval %d \n", parameters->weightInterval);
    printf("AbsoluteError is %d, Relative Error is %d \n", parameters->maximumError, parameters->relativeError);
    printf("PRECEDING BANDS: %d \n", parameters->precedingBands);
    printf("####################ENCODER PARAMETERS############################## \n");
    printf(" UMAX : %d \n", parameters->uMax);
    printf("Counter init %d, Counterrescaling %d \n", parameters->initialY, parameters->yStar);
    printf(" Accumulator init %d \n", parameters->initialK);
    printf("#################################################################### \n");

}
