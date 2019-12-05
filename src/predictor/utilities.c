#include "include/predictor.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

int64_t modR(int64_t x, int64_t R){
    int64_t power2 = ((int64_t)0x1) << (R - 1);
    return ((x + power2) - (((((x + power2) >> (R - 1) >> 1)) << (R - 1)) << 1)) - power2;
}

int offset(uint16_t x, uint16_t y, uint16_t z, struct arguments * args) {
    return (z * args->xSize * args->ySize) + (y * args->xSize) + x;
}

int sgn(int64_t x) {
    if (x > 0) {
        return 1;
    } else if(x < 0) {
        return -1;
    } else {
        return 0;
    }
}

int64_t clip( int64_t x, int64_t xmin, int64_t xmax)  {
    if (x > xmax) {
        return xmax;
    } else if(x < xmin) {
        return xmin;
    } else {
        return x;
    }
}
