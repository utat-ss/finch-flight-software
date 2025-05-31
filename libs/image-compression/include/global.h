#pragma once

#define IMG_Z 3
#define IMG_Y 5
#define IMG_X 5

/*
 * [3.3] Dynamic range of data samples.
 * 2 <= D <= 32
 */
extern int const D;
extern int const Smin;
extern int const Smax;
extern int const Smid;

/*
 * [4.2] Number of preceding spectral bands used for prediction.
 * 0 <= P <= 15
 */
extern int const P;
inline int Pz(int z) { return (z < P) ? z : P; }

/*
 * [4.3] Number of local difference values used for prediction at each sample
 * in band z.
 */
inline int Cz(int z) { return Pz(z) + 3; }

/*
 * [4.6.1.2] Resolution of the weight values.
 * 4 <= Omega <= 19
 */
extern int const Omega;

/*
 * [4.7.2] Register size.
 * max{32, D + Omega + 2} <= R <= 64.
 */
extern int const R;

/*
 * [5.4.3.2.2.2] Limit of codeword size.
 * 8 <= Umax <= 32
 */
extern int const Umax;

typedef enum {
	LocalSum_WIDE_NEIGHBOR,
	LocalSum_NARROW_NEIGHBOR,
	LocalSum_WIDE_COLUMN,
	LocalSum_NARROW_COLUMN
} LocalSumType;

extern LocalSumType LOCAL_SUM_TYPE;
