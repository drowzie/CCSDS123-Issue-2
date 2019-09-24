#include "include/utilities.h"

long long modR(long long x, long long R){
    long long power2 = ((long long)0x1) << (R - 1);
    return ((x + power2) - (((((x + power2) >> (R - 1) >> 1)) << (R - 1)) << 1)) - power2;
}