#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

const char *argp_program_version = "0.0";
const char *argp_program_bug_address = "<chrisboltern3@gmail.com>";
static char doc[] = "CCSDS 123 Issue 2";
static char args_doc[] = "";

static struct argp_option options[] = { 
    { "Full Prediction Mode", 'f', 0, OPTION_ARG_OPTIONAL, "Calculate in Full Prediction Mode (DEFAULT=REDUCED)"},
    { "DECOMPRESSION", 778, 0, 0, "DECOMPRESSION MODE. Input image is the compressed image"},
    { "inputFILE", 'i', "FILE", 0, "FILENAME"},
    { "SIGNED", 788, 0, 0, "SIGNED PIXELS"},
    { "imageorder", 'o',  "ORDER", 0 , "0=BSQ, 1=BIP or 2=BIL"},
    { "imageorder", 'O',  "ENCODEORDER", 0 , "0=BSQ, 1=BIP or 2=BIL"},
    { "Dynamic Range", 'd', "DYNRANGE", 0, "Register D size, #2-16"},
    { "Register size", 'r', "REGSIZE", 0, "Register R size, max{32, D +  Ω +  2} ≤ R ≤ 64"},
    { "Sample resolution", 's', "SRES", 0, "Sample Resolution(Θ), #0-4"},
	{ "Prediction Bands", 'p', "PBANDS", 0, "How many preceding spectral bands to use. #0-16"},
    { "Weight resolution", 'w', "WRES", 0, "Weight Resolution(Omega), #4-19"},
    { "Weight interval", 't', "Winterval", 0, "Chapter 4.8.2, CCSDS Issue 2"},
    { "Vmin", 'v', "vMin", 0, "Chapter 4.8.2, CCSDS Issue 2"},
    { "Vmax", 'V', "vMax", 0, "Chapter 4.8.2, CCSDS Issue 2"},
    { "xSize", 'x', "xSIZE", 0, "x size of image"},
    { "ySize", 'y', "ySIZE", 0, "y size of image"},
    { "zSize", 'z', "zSIZE", 0, "z size of image"},
    { 0 } 
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    int order;
    switch (key) {
    case 'd': arguments->dynamicRange = atoi(arg); break;
    case 'o': 
        arguments->imageOrder = atoi(arg);
        break;
    case 'O': 
        arguments->encodeOrder = atoi(arg);
        break;
    case 788: arguments->pixelType = SIGNED; break;
    case 'i': arguments->fileName = arg; break;
    case 778: arguments->compressionMode = DECOMPRESS; break;
    case 'r': arguments->registerSize = atoi(arg); break;
    case 'f': arguments->mode = FULL; break;
	case 'p': arguments->precedingBands = atoi(arg) < 0 ? 0 : atoi(arg) > 15 ? 15 : atoi(arg)  ; break;
    case 'x': arguments->xSize = atoi(arg); break;
    case 'y': arguments->ySize = atoi(arg); break;
    case 'z': arguments->zSize = atoi(arg); break;
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void parseArguments(int argc, char **argv, struct arguments * arguments) {
    // Prediction specific
	arguments->mode = REDUCED;
    arguments->dynamicRange = 16;
	arguments->precedingBands = 1;
    arguments->weightResolution = 14;
    arguments->weightMin = -1;
    arguments->weightMax = 1;
    arguments->weightInterval = 5;
    arguments->sumType = NEIGHBOR;
    // Encoder specifics
    arguments->uMax = 18;
    arguments->initialK = 7;
    arguments->initialY = 1;
    arguments->wordSize = 8; // The computer word size
    arguments->sMin = 0;
    arguments->sMax = (0x1 << arguments->dynamicRange) - 1;
    arguments->sMid = 0x1 << (arguments->dynamicRange - 1);
    arguments->compressionMode = COMPRESS;
    arguments->pixelType = UNSIGNED;
    arguments->imageOrder = BIP;
    arguments->encodeOrder = BIP;
    arguments->registerSize = 64;
    arguments->yStar=6;
    argp_parse(&argp, argc, argv, 0, 0, arguments);
}
