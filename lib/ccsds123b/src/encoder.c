/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/ccsds123b/arrays.h>
#include <finch/ccsds123b/constants.h>
#include <finch/ccsds123b/encoder.h>
#include <finch/ccsds123b/util.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>

static void compute_k(int accumulator, int counter, int *k)
{
	if (2 * counter > accumulator + (int32_t)(49/pow(2, 7) * counter)) {
		*k = 0;
	} else {
		for (*k = 1; *k <= D - 2; ++(*k)) {
			if (counter * pow(2, *k) > accumulator + (int32_t)(49/pow(2, 7) * counter)) {
				*k -= 1;
				break;
			}
		}
	}
}

void encode_prediction(const vec3 *N, Predictions prediction, EncoderOut out)
{
	for (int z = 0; z < N->z; ++z) {
		int kz = 0;
		int accumulator = 1.0 / pow(2, 7) * (3 * pow(2, kz + 6) - 49) * 2;
		int counter = 2;

		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x) {
				if (y != 0 && x != 0) {
					if (counter < pow(2, 4) - 1) {
						accumulator += get_predictions(prediction, z, y, x);
					} else if (counter == pow(2, 4) - 1) {
						int32_t p = get_predictions(prediction, z, y - 1, N->x - 1);

						accumulator = (accumulator + p + 1) / 2.0f;
					}

					if (counter < pow(2, 4) - 1) {
						counter++;
					} else {
						counter = (counter + 1) / 2.0f;
					}
				}

				// [5.4.3.2.2] Sample adaptive entropy encoder
				int32_t j = get_predictions(prediction, z, y, x);
				int32_t k;

				compute_k(accumulator, counter, &k);
				encode_gpo2(j, k, get_encoder_out(out, z, y, x));
			}
		}
	}
}

void encode_gpo2(int j, int k, EncoderOutItem out)
{
	if (j >= (1U << D) || k > D) {
		snprintf(out, 32, "%s", "<neg>");
		return;
	}

	unsigned int prefix = j >> k;
	unsigned int suffix = j & ((1 << k) - 1);
	int index = 0;

	if (prefix < Umax) {
		for (int i = k - 1; i >= 0; --i) {
			out[index++] = (suffix & (1 << i)) ? '1' : '0';
		}

		out[index++] = '1';

		for (unsigned int i = 0; i < prefix; ++i) {
			out[index++] = '0';
		}
	} else {
		for (int i = D - 1; i >= 0; --i) {
			out[index++] = (j & (1 << i)) ? '1' : '0';
		}

		for (unsigned int i = 0; i < Umax; ++i) {
			out[index++] = '0';
		}
	}

	out[index] = '\0';
}
