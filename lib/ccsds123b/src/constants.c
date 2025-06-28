#include <finch/ccsds123b/constants.h>

/* The values chosen here are temporary and only used for testing
 * purposes.
 */
int const D = 7;

int const Smin;
int const Smax = (1 << D) - 1;
int const Smid = 1 << (D - 1);

int const P = 3;

int const Omega = 10;

int const R = 32;

int const THETA = 4;
int const PHI;
int const PSI;

int const Umax = 8;

LocalSumType LOCAL_SUM_TYPE = LocalSum_WIDE_NEIGHBOR;
