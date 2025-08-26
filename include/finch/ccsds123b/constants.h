/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_CONSTANTS_H
#define FINCH_INCLUDE_CONSTANTS_H

/*
 * [3.3] Dynamic range of data samples.
 * 2 <= D <= 32
 * Smin = 0
 * Smax = 2^D - 1
 * Smid = 2^(D - 1)
 */
extern int const D;
extern int const Smin;
extern int const Smax;
extern int const Smid;

/*
 * [4.2] Number of preceding spectral bands used for prediction.
 * Full prediction mode.
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
 * [4.9.1] Resolution parameter
 * 0 <= THETA <= 4
 * 0 <= PHI <= 2^THETA - 1
 * 0 <= PSI <= 2^THETA - 1
 */
extern int const THETA;
extern int const PHI;
extern int const PSI;

/*
 * [5.4.3.2.2.2] Limit of codeword size.
 * 8 <= Umax <= 32
 */
extern int const Umax;

typedef enum {
	LocalSum_WIDE_NEIGHBOR,
	LocalSum_NARROW_NEIGHBOR,
} LocalSumType;

extern LocalSumType LOCAL_SUM_TYPE;

#define N_TILE_Z 10
#define N_TILE_Y 10
#define N_TILE_X 5

#define N_Z 50
#define N_Y 50
#define N_X 50

#endif /* FINCH_INCLUDE_CONSTANTS_H */
