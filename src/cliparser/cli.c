#include "include/cli.h"
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
    { "debug", 777, 0, 0, "DEBUG MODE"},
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
    { "inputFILE", 'i', "FILE", 0, "FILENAME"},
    
    { 0 } 
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'd': arguments->dynamicRange = atoi(arg); break;
    case 'i': arguments->fileName = arg; break;
    case 777: arguments->debugMode = 1; break;
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
    // Image sizes, defaults to test pattern
    arguments->xSize = 4;
    arguments->ySize = 4;
    arguments->zSize = 2;
    // Prediction specific
	arguments->mode = REDUCED;
    arguments->dynamicRange = 16;
	arguments->precedingBands = 15;
    arguments->weightResolution = 13;
    arguments->theta = 0;
    arguments->weightMin = -1;
    arguments->weightMax = 3;
    arguments->weightInterval = 6;
    arguments->debugMode = 0;
    // Encoder specifics
    arguments->uMax = 16;
    arguments->initialK = 5;
    arguments->initialY = 1;
    arguments->wordSize = 4;
    arguments->sMin = 0;
    arguments->sMax = (0x1 << arguments->dynamicRange) - 1;
    arguments->sMid = 0x1 << (arguments->dynamicRange - 1);

    // Hybrid Encoder
    arguments->initialAccumulator = 1<<6;
    argp_parse(&argp, argc, argv, 0, 0, arguments);
    // Constraint defauluts based on previous arguments, this is limitations from the CCSDS 123 Blue book.
    arguments->precedingBands = arguments->zSize > arguments->precedingBands ? arguments->precedingBands : arguments->zSize;
    //arguments->precedingBands = arguments->precedingBands > 15 ? 15 : arguments->precedingBands < 0 ? 0 : arguments->precedingBands;
    arguments->registerSize = 32 > (arguments->dynamicRange+arguments->weightResolution + 2) ? 32 : (arguments->dynamicRange+arguments->weightResolution + 2);
    if(arguments->yStar > 9) {
        arguments->yStar = 9;
    } else {
        if (4 > arguments->initialY) {
            arguments-> yStar = 4;
        } else {
            arguments->yStar = arguments->initialY + 1;
        }
    }
    arguments->yStar=6;
}
