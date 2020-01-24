#pragma once
#include <inttypes.h>

struct arguments {
    //NAME
    char *fileName;
    //
    enum { FULL, REDUCED } mode;
    int precedingBands;
    // User parameters
    unsigned char dynamicRange; // D register
    unsigned char registerSize; // R register
    // Sample represantation
    int theta;
    // Weight parameters
    unsigned char weightResolution;
    int weightInterval;
    int weightMin;
    int weightMax;
    // Image Parameters
    int xSize;
    int ySize;
    int zSize;
    // Encoding stuff
    char initialY;
    char yStar;
    unsigned int initialK;
    unsigned int uMax;
    // Hybrid encoder specific
    uint64_t initialAccumulator;
    unsigned int wordSize;
    //
    int32_t  sMin;
    int32_t  sMax;
    int32_t  sMid;

    int debugMode;
};

extern struct arguments parameters;

void parseArguments(int argc, char **argv, struct arguments * arguments);