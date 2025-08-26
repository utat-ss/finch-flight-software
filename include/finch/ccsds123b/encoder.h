/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_ENCODER_H
#define FINCH_INCLUDE_ENCODER_H

#include <finch/ccsds123b/arrays.h>
#include <finch/ccsds123b/constants.h>
#include <finch/ccsds123b/util.h>

/*
 * [5.4.2.3] Band Sequential Order.
 */
void encode_prediction(
	const vec3 * N,
	Predictions prediction,
	EncoderOut encoding
);

/*
 * [5.4.3.2.2.1] Length limited Golomb-power-of-2 (GPO2)
 */
void encode_gpo2(
	int j, int k,
	EncoderOutItem out
);

#endif /* FINCH_INCLUDE_ENCODER_H */
