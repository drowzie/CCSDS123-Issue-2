#include "uthash.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char activePrefix[16][256];

int encodeLowEntropy(uint32_t treshold, char *input) {
    if (treshold <= 303336 && treshold > 225404) {
        strcat(activePrefix[15], input);
    } else if (treshold <= 225404 && treshold > 166979) {
        strcat(activePrefix[14], input);
    } else if (treshold <= 166979 && treshold > 128672) {
        strcat(activePrefix[13], input);
    } else if (treshold <= 128672 && treshold > 95597) {
        strcat(activePrefix[12], input);
    } else if (treshold <= 95597 && treshold > 69670) {
        strcat(activePrefix[11], input);
    } else if (treshold <= 69670 && treshold > 50678) {
        strcat(activePrefix[10], input);
    } else if (treshold <= 50678 && treshold > 34898) {
        strcat(activePrefix[9], input);
    } else if (treshold <= 34898 && treshold > 23331) {
        strcat(activePrefix[8], input);
    } else if (treshold <= 23331 && treshold > 14935) {
        strcat(activePrefix[7], input);
    } else if (treshold <= 14935 && treshold > 9282) {
        strcat(activePrefix[6], input);
    } else if (treshold <= 9282 && treshold > 5510) {
        strcat(activePrefix[5], input);
    } else if (treshold <= 5510 && treshold > 3195) {
        strcat(activePrefix[4], input);
    } else if (treshold <= 3195 && treshold > 1928) {
        strcat(activePrefix[3], input);
    } else if (treshold <= 1928 && treshold > 1112) {
        strcat(activePrefix[2], input);
    } else if (treshold <= 1112 && treshold > 408) {
        strcat(activePrefix[1], input);
    } else {
        strcat(activePrefix[0], input);
    }
    return 0;
}

int main() {
    encodeLowEntropy(0, "0");
    encodeLowEntropy(0, "X");
    printf("test %s\n", activePrefix[0]);
    return 0;
}
