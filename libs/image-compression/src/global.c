#include "global.h"

int const D = 7;

int const Smin = 0;
int const Smax = (1 << D) - 1;
int const Smid = 1 << (D - 1);

int const P = 3;

int const Omega = 10;

int const R = 32;

int const Umax = 8;

LocalSumType LOCAL_SUM_TYPE = LocalSum_WIDE_NEIGHBOR;
