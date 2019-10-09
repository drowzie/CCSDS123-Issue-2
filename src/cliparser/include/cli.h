#pragma once

struct arguments {
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
    char weightMin;
    char weightMax;
    // Image Parameters
    int xSize;
    int ySize;
    int zSize;
};

void parseArguments(int argc, char **argv, struct arguments * arguments);