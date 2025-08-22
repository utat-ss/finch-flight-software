/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "reconstructor.h"
#include <finch/ccsds123b/constants.h>

#include <string.h>
#include <math.h>

void decode_encoding(
	const vec3 * N,
	char encoding[N->z][N->y][N->x][32],
	int prediction[N->z][N->y][N->x])
{
	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x) {
				int decoded_j;
				int decoded_k;

				// Decode the GPO2 string to get the original prediction value 'j'
				decode_gpo2(encoding[z][y][x], &decoded_j, &decoded_k);

				// Store the decoded 'j' value into the prediction array
				prediction[z][y][x] = decoded_j;
			}
		}
	}
}

void decode_gpo2(const char in[32], int *j, int *k)
{
	if (strcmp(in, "<neg>") == 0) {
		*j = -1;
		*k = -1;
		return;
	}

		// Determine k and prefix based on the structure of the encoded string
	// The suffix is the initial 'k' bits
	// The '1' after the suffix marks the end of the suffix and the start of the prefix '0's
	// The number of '0's after the '1' determines the prefix value

	unsigned int suffix_val = 0;
	int suffix_len = 0;
	int index = 0;

	// Read the suffix bits (until '1' is encountered)
	while (in[index] == '0' || in[index] == '1') {
		// Check if the current character is '1', which marks the end of suffix bits
		if (in[index] == '1') {
			// This '1' is part of the prefix-marker, not the suffix bits.
			// Suffix bits are inverted in order in the string.
			// Suffix length is the number of bits before this '1'.
			suffix_len = index;
			break;
		}
		index++;
	}

	// Reconstruct the suffix value
	for (int i = 0; i < suffix_len; ++i) {
		if (in[i] == '1') {
			suffix_val |= (1 << (suffix_len - 1 - i));
		}
	}

	// Move past the '1' marker
	index++;

	unsigned int prefix_val = 0;

	while (in[index] == '0') {
		prefix_val++;
		index++;
	}

	*k = suffix_len;
	*j = (prefix_val << *k) | suffix_val;

	if (*k == 0 && prefix_val == Umax && in[0] != '1') {
		int direct_j_len = 0;
		int current_idx = 0;

		while (in[current_idx] == '0' || in[current_idx] == '1') {
			if (current_idx >= D) {
				break;
			}
			current_idx++;
		}
		direct_j_len = current_idx;

		if (direct_j_len <= D) {
			int zeros_count = 0;

			while (in[current_idx + zeros_count] == '0') {
				zeros_count++;
			}

			if (zeros_count == Umax && in[current_idx + zeros_count] == '\0') {
				int direct_j = 0;

				for (int i = 0; i < D; ++i) {
					if (in[i] == '1') {
						direct_j |= (1 << (D - 1 - i));
					}
				}
				*j = direct_j;
				*k = D;
				return;
			}
		}
	}
}

void reconstruct_prediction(
	const vec3 * N,
	int prediction[N->z][N->y][N->x],
	int image[N->z][N->y][N->x])
{
	LocalDiff local_diff[N->z][N->y][N->z];

	int weights[P + 3];

	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x) {
				image[z][y][x] = prediction[z][y][x];
			}
		}
	}

	for (int z = 0; z < N->z; ++z) {
		initialize_weights(weights, z, Omega);

		int t = 0;

		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x, ++t) {
				int64_t mapped_quantizer_index = image[z][y][x];

				// Step 1: Compute predicted_sample_value (needed for unmapping)
				int32_t local_sum = compute_local_sum(z, y, x);

				local_diff[z][y][x] = compute_local_diffs(z, y, x, local_sum);

				int32_t pred_cent_local_diff = compute_pred_cent_local_diff(z, y, x, local_diff, weights);
				int64_t high_res_pred_sample = compute_high_res_pred_sample(pred_cent_local_diff, local_sum, Omega, R, Smid, Smax, Smin);
				int64_t double_res_pred_sample = double_resolution_predicted_sample(high_res_pred_sample, z, t);
				int64_t predicted_sample_value = predicted_sample(double_res_pred_sample);

				// Step 2: Reverse compute_mapped_quantizer_index to get quantizer_index
				int64_t theta = fmin(predicted_sample_value - Smin, Smax - predicted_sample_value);
				int64_t quantizer_index;

				if (mapped_quantizer_index > 2 * theta) {
					quantizer_index = mapped_quantizer_index - theta; // Case: abs(quantizer_index) > theta
				} else {
					int64_t is_even = mapped_quantizer_index % 2 == 0;
					int64_t abs_q = is_even ? mapped_quantizer_index / 2 : (mapped_quantizer_index + 1) / 2;
					int64_t d = double_res_pred_sample % 2; // Parity from double_res_pred_sample

					quantizer_index = is_even ? abs_q : -abs_q;
					quantizer_index *= (d == 0 ? 1 : -1); // Apply sign based on parity
				}

				// Step 3: Reverse compute_quantizer_index to get prediction_residual
				int64_t prediction_residual;

				if (x == 0 && y == 0) {
					prediction_residual = quantizer_index;
				} else {
					int sgn = (quantizer_index >= 0) ? 1 : -1;

					prediction_residual = sgn * fabs(quantizer_index);
				}

				int64_t sample_value = predicted_sample_value + prediction_residual;

				if (sample_value < Smin) {
					sample_value = Smin;
				} else if (sample_value > Smax) {
					sample_value = Smax;
				}

				image[z][y][x] = sample_value;
			}
		}
	}
}
