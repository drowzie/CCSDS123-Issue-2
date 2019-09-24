void wideNeighborLocalSum(int * sample, int * localsum);
void narrowNeighborLocalSum(int * sample, int * localsum, int smid);
void wideColumnLocalSum(int * sample, int * localsum);
void narrowColumnLocalSum(int * sample, int * localsum, int smid);
void centralLocalDiffrence(int * sample, int * localsum, int * centraldiffrence);
void northLocalDiffrence(int * sample, int * localsum, int * northLocalDiffrence);

void westLocalDiffrence(int * sample, int * localsum, int * westLocalDiffrence);
void northwestLocalDiffrence(int * sample, int * localsum, int * northwestLocalDiffrence);

int offset(int x, int y, int z);