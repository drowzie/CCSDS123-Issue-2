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
    { "Dynamic Range", 'd', "DYNRANGE", 0, "Register D size, #2-16"},
    { "Register size", 'r', "REGSIZE", 0, "Register R size, max{32, D +  Ω +  2} ≤ R ≤ 64"},
    { "Sample resolution", 's', "SRES", 0, "Sample Resolution(Θ), #0-4"},
	{ "Prediction Bands", 'p', "PBANDS", 0, "How many preceding spectral bands to use. #0-16"},
    { "Weight resolution", 'w', "WRES", 0, "Weight Resolution(Omega), #4-19"},
    { "Weight interval", 't', "Winterval", 0, "Chapter 4.8.2, CCSDS Issue 2"},
    { "Vmin", 'v', "vMin", 0, "Chapter 4.8.2, CCSDS Issue 2"},
    { "Vmax", 'm', "vMax", 0, "Chapter 4.8.2, CCSDS Issue 2"},
    { "xSize", 'x', "xSIZE", 0, "x size of image"},
    { "ySize", 'y', "ySIZE", 0, "y size of image"},
    { "zSize", 'z', "zSIZE", 0, "z size of image"},
    
    { 0 } 
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'd': arguments->dynamicRange = atoi(arg); break;
    case 'r': arguments->registerSize = atoi(arg); break;
    case 'f': arguments->mode = FULL; break;
	case 'p': arguments->precedingBands = atoi(arg) < 0 ? 0 : atoi(arg) > 16 ? 16 : atoi(arg)  ; break;
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
	arguments->mode = REDUCED;
    arguments->dynamicRange = 2;
	arguments->precedingBands = 0;
    arguments->weightResolution = 0;
    arguments->theta = 0;
    arguments->xSize = 3;
    arguments->ySize = 3;
    arguments->zSize = 2;
    arguments->weightMin = 2;
    arguments->weightMax = 4;
    arguments->weightInterval = 10000;
    argp_parse(&argp, argc, argv, 0, 0, arguments);
    arguments->registerSize = 32 > arguments->dynamicRange ? 32 : arguments->dynamicRange;
}
