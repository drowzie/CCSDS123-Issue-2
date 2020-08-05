#pragma once
#include <inttypes.h>

struct arguments {

    /*
      General Parameters
    */
    enum endianessEnum {LE = 0, BE = 1} endianess;
    int wordSize;
    int pixelSize;
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
    unsigned int dynamicRange; // D register
    uint16_t registerSize; // R register
    // Local sum type
    enum {WIDENEIGHBOR= 0,NARROWNEIGHBOR = 1, WIDECOLUMN = 2, NARROWCOLUMN = 3} sumType;
    // Weight parameters
    uint32_t weightResolution;
    uint16_t weightInterval;
    int16_t weightMin;
    int16_t weightMax;
    // Lossy compression parameters
    enum fidelityControlEnum {LOSSLESS = 0, ABSOLUTE = 1, RELATIVE = 2, ABSREL = 3} fidelityControl;
    int16_t theta;
    uint32_t maximumError;
    uint32_t relativeError;
    uint32_t sampleDamping;
    uint32_t sampleOffset;
    uint32_t interbandOffset;
    uint32_t intrabandExponent;
    // Image Parameters
    int16_t xSize;
    int16_t ySize;
    int16_t zSize;
    // Encoding stuff
    int16_t initialY;
    int16_t yStar;
    uint16_t initialK;
    uint16_t uMax;
    // Hybrid encoder specific
    uint64_t initialAccumulator;
    // defined by the dynamicRange
    uint64_t sMin;
    uint64_t sMax;
    uint64_t sMid;

    int debugMode;
};

extern struct arguments parameters;

void parseArguments(int argc, char **argv, struct arguments * arguments);
void printParameters(struct arguments * parameters);
