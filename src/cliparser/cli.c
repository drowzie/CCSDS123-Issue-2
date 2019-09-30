#include "include/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

const char *argp_program_version = "0.0";
const char *argp_program_bug_address = "<chrisboltern3@gmail.com>";
static char doc[] = "CCSDS 123 Issue 2";
static char args_doc[] = "[FILENAME]...";

static struct argp_option options[] = { 
    { "Full Prediction Mode", 'f', 0, 0, "Calculate in Full Prediction Mode"},
	{ "Prediction Bands", 'p', "PBANDS", 0, "How many preceding spectral bands to use. Any number between 0 to 16"},
    { "x", 'xSize', "xSIZE", 0, "x size of image"},
    { "y", 'ySize', "ySIZE", 0, "y size of image"},
    { "z", 'zSize', "zSIZE", 0, "z size of image"},
    
    { 0 } 
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
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
	arguments->precedingBands = 0;
    arguments->xSize = 3;
    arguments->ySize = 3;
    arguments->zSize = 2;
    argp_parse(&argp, argc, argv, 0, 0, arguments);
}