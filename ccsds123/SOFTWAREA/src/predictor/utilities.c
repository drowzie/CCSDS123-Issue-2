#include "predictor.h"
#include "cliparser/cli.h"
#include <stdio.h>


int64_t modR(int64_t x, int64_t R){
    int64_t power2 = ((int64_t)0x1) << (R - 1);
    return ((x + power2) - (((((x + power2) >> (R - 1) >> 1)) << (R - 1)) << 1)) - power2;
}

/* 
    Offset for reading the correct order of the image
*/
int offset(uint16_t x, uint16_t y, uint16_t z, struct arguments * args) {
    if(args->imageOrder == BSQ) {
        return (z * args->xSize * args->ySize) + (y * args->xSize) + x; // WHEN READING BSQ
    } else if (args->imageOrder == BIP) {
        return (args->xSize*args->zSize*y) + (args->zSize*x) + z; // WHEN READING BIP
    } else if (args->imageOrder == BIL) {
        return (args->xSize*args->zSize*y) + (args->xSize*z) + x; // WHEN READING BIL
    }
}

inline int sgn(int64_t x) {
    if (x > 0) {
        return 1;
    } else if(x < 0) {
        return -1;
    } else {
        return 0;
    }
}

inline int sgnPlus(int64_t x) {
    if (x >= 0) {
        return (1);
    } else {
        return (-1);
    }
}
