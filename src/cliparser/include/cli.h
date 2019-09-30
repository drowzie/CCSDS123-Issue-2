#pragma once

struct arguments {
    enum { FULL, REDUCED } mode;
    int precedingBands;
    int xSize;
    int ySize;
    int zSize;
};

void parseArguments(int argc, char **argv, struct arguments * arguments);