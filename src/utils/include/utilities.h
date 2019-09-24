#define sgn(x) ((x) > (0) ?  1 : ((x) < 0 ? -1 : 0))

#define sgnplus(x) ((x) >= (0) ? 1 : -1)

#define clip(x, xmin, xmax) (((x) < (xmin)) ? (xmin) : ((x) > (xmax) ? xmax : x))

#define modR(x, R) ((int) (x + pow(2,R-1)) % (int) pow(2,R) - (int) pow(2,R-1))