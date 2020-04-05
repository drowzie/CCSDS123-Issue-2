#pragma once
#include <inttypes.h>

struct arguments {
    //NAME
    char *fileName;
    enum imageTypeEnum {BSQ = 0, BIP, BIL} imageOrder;
    enum imageTypeEnum encodeOrder;
    enum { COMPRESS = 0, DECOMPRESS = 1} compressionMode;
    //
    enum { FULL = 0, REDUCED = 1 } mode;
    enum { UNSIGNED = 0, SIGNED = 1 } pixelType;
    int precedingBands;
    // User parameters
    unsigned char dynamicRange; // D register
    unsigned char registerSize; // R register
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
    unsigned int wordSize;
    //
    int32_t  sMin;
    int32_t  sMax;
    int32_t  sMid;
};

extern struct arguments parameters;

void parseArguments(int argc, char **argv, struct arguments * arguments);