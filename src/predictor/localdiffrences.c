#include "include/predictor.h"
#include "../utils/include/utilities.h"
#include "../config/include/config.h"
#include "../cliparser/include/cli.h"
#include <stdio.h>

/*
	Chapter 4.5 in CCSDS 123 Issue 2
*/

void centralLocalDiffrence(int * sample, int * localsum, int * centraldiffrence, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		centraldiffrence[offset(x,y,z,args)] = ((4 *  sample[offset(x,y,z,args)]) - localsum[offset(x,y,z,args)]);
	}
}


void northLocalDiffrence(int * sample, int * localsum, int * northLocalDiffrence, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if (y > 0) {
			northLocalDiffrence[offset(x,y,z,args)] = ((4 * sample[offset(x,y-1,z, args)]) - localsum[offset(x,y,z,args)]);
		} else if (y == 0) {
			northLocalDiffrence[offset(x,y,z,args)] = 0;
		} else {
			printf("northLocalDiffrence should not arrive here\n");
		}
	}
}

void westLocalDiffrence(int * sample, int * localsum, int * westLocalDiffrence, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if ( x > 0 && y > 0) {
			westLocalDiffrence[offset(x,y,z,args)] = ((4 * sample[offset(x-1,y,z, args)]) - localsum[offset(x,y,z,args)]);
		} else if (x == 0 && y > 0) {
			westLocalDiffrence[offset(x,y,z,args)] = ((4 * sample[offset(x,y-1,z, args)]) - localsum[offset(x,y,z,args)]);
		} else if (y == 0) {
			westLocalDiffrence[offset(x,y,z,args)] = 0;
		} else {
			printf("westLocalDiffrence should not arrive here\n");
		}
	}
}

void northwestLocalDiffrence(int * sample, int * localsum, int * northwestLocalDiffrence, int x, int y, int z, struct arguments * args) {
	if(!(x == 0 && y == 0)) {
		if (x > 0 && y > 0) {
			northwestLocalDiffrence[offset(x,y,z,args)] = ((4* sample[offset(x-1,y-1,z, args)]) - localsum[offset(x,y,z,args)]);
		} else if (x == 0 && y > 0) {
			northwestLocalDiffrence[offset(x,y,z,args)] = ((4* sample[offset(x,y-1,z, args)]) - localsum[offset(x,y,z,args)]);
		} else if (y == 0) {
			northwestLocalDiffrence[offset(x,y,z,args)] = 0;
		} else {
			printf("northwestLocalDiffrence should not arrive here\n");
		}
	}
}