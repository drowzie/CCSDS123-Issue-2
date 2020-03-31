#pragma once
#include <inttypes.h>

struct arguments {
    //NAME
    char *fileName;
    enum imageTypeEnum {BSQ = 0, BIP, BIL} imageOrder;
    enum imageTypeEnum encodeOrder;
    //
    enum { FULL = 0, REDUCED =1 } mode;
    enum { UNSIGNED = 0, SIGNED = 1} pixelType;
    enum { COMPRESS = 0, DECOMPRESS = 1} compressionMode;
    int precedingBands;
    // User parameters
    unsigned char dynamicRange; // D register
    unsigned char registerSize; // R register
    // Weight parameters
    unsigned char weightResolution;
    int weightInterval;
    int weightMin;
    int weightMax;
    // Lossy compression parameters
    int theta;
    uint32_t maximumError;
    uint32_t sampleDamping;
    uint32_t sampleOffset;
    uint32_t interbandOffset;
    int32_t intrabandExponent;
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
    // defined by the dynamicRange
    int32_t  sMin;
    int32_t  sMax;
    int32_t  sMid;

    int debugMode;
};

extern struct arguments parameters;

void parseArguments(int argc, char **argv, struct arguments * arguments);