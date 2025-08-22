/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_ENCODER_H
#define FINCH_INCLUDE_ENCODER_H

#include <finch/ccsds123b/util.h>
#include <finch/ccsds123b/constants.h>

/*
 * [5.4.2.3] Band Sequential Order.
 */
void encode_prediction(
	const vec3 * N,
	int32_t prediction[N->z][N->y][N->x],
	char encoding[N->z][N->y][N->x][32]
);

/*
 * [5.4.3.2.2.1] Length limited Golomb-power-of-2 (GPO2)
 */
void encode_gpo2(
	int j, int k,
	char out[32]
);

#endif /* FINCH_INCLUDE_ENCODER_H */
