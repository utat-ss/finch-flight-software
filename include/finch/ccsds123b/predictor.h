/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_PREDICTOR_H
#define FINCH_INCLUDE_PREDICTOR_H

#include <finch/ccsds123b/arrays.h>
#include <finch/ccsds123b/constants.h>
#include <finch/ccsds123b/util.h>

#include <stdint.h>

/*
 * Returns the prediction of dimension N
 */
void predict_image(
	const vec3 * N,
	Predictions p
);

/*
 * [4.4] Local Sum
 *
 * Not defined at t=0 as it is not needed.
 */
int32_t compute_local_sum(
	int z,
	int y,
	int x
);

/*
 * [4.5.1] Central Differences
 * [4.5.2] Directional Differences
 *
 * Not defined at t=0 as it is not needed.
 */
LocalDiff compute_local_diffs(
	int z,
	int y,
	int x,
	int32_t local_sum
);

/*
 * [4.7.1] Predicted Central Local Difference
 *
 * Not defined at t=0 as it is not needed.
 */
int32_t compute_pred_cent_local_diff(
	int32_t z,
	int32_t y,
	int32_t x,
	LocalDiffs local_diffs,
	const int32_t *weights
);

/*
 * [4.6.3.2] Weight Initialization
 */
void initialize_weights(
	int32_t *weights,
	int32_t weights_size,
	int z,
	int32_t omega
);

int32_t update_weight(int32_t omega_t, int32_t e_z_t, int32_t p_t, int32_t xi_z_i,
	int32_t d_z_i_t1, int32_t omega_min, int32_t omega_max);

/*
 * [4.7.2] High Resolution Predicted Sample
 */
int64_t compute_high_res_pred_sample(int32_t pred_cent_local_diff, int32_t local_sum, int32_t Omega, int32_t R, int32_t Smid, int32_t Smax, int32_t Smin);

/*
 * [4.7.3] Double Resolution Predicted Sample
 */
int64_t double_resolution_predicted_sample(int64_t high_res_pred_sample_val, int z, int t);

/*
 * [4.7.4] Predicted Sample
 */
int64_t predicted_sample(int64_t double_res_pred_sample);

/*
 * [4.8.1] Prediction Residual
 */
int64_t compute_prediction_residual(int64_t sample_value, int64_t predicted_value);

/*
 * [4.8.1] Quantizer Index
 */
int64_t compute_quantizer_index(int64_t prediction_residual);

/*
 * [4.11] Mapped Quantizer Index
 */
int64_t compute_mapped_quantizer_index(int64_t quantizer_index, int64_t double_res_pred_sample_value, int64_t predicted_sample_value);

#endif /* FINCH_INCLUDE_PREDICTOR_H */
