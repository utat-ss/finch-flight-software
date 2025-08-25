/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_RECONSTRUCTOR_H
#define FINCH_INCLUDE_RECONSTRUCTOR_H

#include <finch/ccsds123b/predictor.h>
#include <finch/ccsds123b/constants.h>
#include <finch/ccsds123b/util.h>

/*
 * Outputs the predicted image.
 */
void decode_encoding(
	const vec3 * N,
	char encoding[N->z][N->y][N->x][32],
	int prediction[N->z][N->y][N->x]
);

/*
 * Decodes a single codeword.
 */
void decode_gpo2(
	const char code[32],
	int *j,
	int *k
);

/*
 * Outputs the original image.
 */
void reconstruct_prediction(
	const vec3 * N,
	int prediction[N->z][N->y][N->x],
	int image[N->z][N->y][N->x]
);

#endif /* FINCH_INCLUDE_RECONSTRUCTOR_H */
