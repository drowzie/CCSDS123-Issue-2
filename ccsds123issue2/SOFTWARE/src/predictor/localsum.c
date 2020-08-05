#include "include/predictor.h"
#include "cliparser/include/cli.h"
#include <stdio.h>

/*
	Chapter 4.4 in CCSDS 123 Issue 2
*/

int64_t wideNeighborLocalSum(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
	if (y == 0 && x > 0){
		return 4LL*(int64_t)(sample[offset(x-1,y,z, arg)]);
	} else if (y > 0 && x > 0 && x < arg->xSize-1) {
		return  (int64_t)(sample[offset(x-1,y,z, arg)] + (int64_t)sample[offset(x-1,y-1,z, arg)] + (int64_t)sample[offset(x,y-1,z, arg)] + (int64_t)sample[offset(x+1,y-1,z, arg)]);
	} else if (y > 0 && x == 0) {
		return (2LL * ((int64_t)sample[offset(x+1,y-1,z, arg)] + (int64_t)sample[offset(x,y-1,z, arg)]));
	} else {
		return ((int64_t)sample[offset(x-1,y,z, arg)] + (int64_t)sample[offset(x-1,y-1,z, arg)] + 2LL * (int64_t)sample[offset(x,y-1,z, arg)]);
	}
}

int64_t narrowNeighborLocalSum(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
	if (y > 0 && x > 0 && x < arg->xSize-1) {
		return	((int64_t)sample[offset(x-1,y-1,z, arg)] +  (2LL * (int64_t)sample[offset(x,y-1,z, arg)]) + (int64_t)sample[offset(x+1,y-1,z, arg)]);
	} else if (y == 0 && x > 0 && y > 0) {
		return	(4LL * (int64_t)sample[offset(x-1,y,z-1, arg)]);
	} else if (y > 0 && x == 0) {
		return	(2LL * ((int64_t)sample[offset(x,y-1,z, arg)] + (int64_t)sample[offset(x+1,y-1,z, arg)]));
	} else if (y > 0 && x == arg->xSize - 1) {
		return	(2LL * ((int64_t)sample[offset(x-1,y-1,z, arg)] + (int64_t)sample[offset(x,y-1,z, arg)]));
	} else {
		return	(4LL * (int64_t)arg->sMid);
	}
}

int64_t wideColumnLocalSum(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
	if(y > 0) {
		return	(4LL*(int64_t)sample[offset(x,y-1,z, arg)]);
	}else {
		return	(4LL*(int64_t)sample[offset(x-1,y,z, arg)]);
	}
}

int64_t narrowColumnLocalSum(uint32_t * sample, uint16_t x, uint16_t y, uint16_t z, struct arguments * arg) {
	if (y > 0) {
		return	(int64_t) (4 * sample[offset(x,y-1,z, arg)]);
	} else if (y == 0 && x > 0 && z > 0) {
		return	(int64_t) (4 * sample[offset(x-1,y,z-1, arg)]);
	} else {
		return	(int64_t) (4 * arg->sMid);
	}
}
