#include "predictor.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/sys/mem_manage.h>
#include <zephyr/kernel.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>

LocalDiff local_diffs[IMG_Z][IMG_Y][IMG_X];

int clip(int value, int min, int max) {
    return (value < min) ? min : (value > max) ? max : value;
}

static int32_t sgn_pos(int x) { if (x > 0) return 1; return -1; }

int32_t prev_quantizer;
int32_t dbl_res;

static int min(int x, int y) { return (x < y) ? x : y; };

static int sign_plus(int x) { return (x >= 0) ? 1 : -1; }

void predict_image(int32_t img[IMG_Z][IMG_Y][IMG_X], int32_t res[IMG_Z][IMG_Y][IMG_X])
{
	vec3 s = (vec3){ IMG_Z, IMG_Y, IMG_X };
	/*
	 * Compute Prediction and Quantization
	 */

	int64_t tmp[IMG_Z][IMG_Y][IMG_X];
	
	for (int i = 0; i < IMG_Z; ++i)
	{
		int32_t weights[Pz(i) + 3];
		initialize_weights(weights, i, Omega);

		int t = 0;
		for (int j = 0; j < IMG_Y; ++j)
		{
			for (int k = 0; k < IMG_X; ++k, ++t)
			{
				int32_t local_sum = compute_local_sum(img, &s, i, j, k);
				local_diffs[i][j][k] = compute_local_diffs(img, i, j, k, local_sum);

				if (i > 0 && t > 0 && t < 3)
				{
					int ez;
					if (k == 0)
						ez = 2 * img[i][j - 1][IMG_X - 1];
					else
						ez = 2 * img[i][j][k - 1];

					int ph = -6 + (int32_t)((t - IMG_X) / 16) + 10 - Omega;
					weights[t + 1] = update_weight(
						weights[t + 1],                   // omega_t: current weight
						ez,                               // e_z_t: prediction error
						ph,                               // p_t: scaling exponent
						0,                                // xi_z_i: default to 0
						local_diffs[i][j][k].central,     // d_z_i_t1: central local difference
						-(1 << (Omega + 2)),              // omega_min: -2^(Omega+2)
						(1 << (Omega + 2)) - 1            // omega_max: 2^(Omega+2) - 1
					);
				}

				/*
				 * Prediction
				 */

				int32_t pred_cent_local_diff =
					compute_pred_cent_local_diff(i, j, k, local_diffs, weights);

				int64_t high_res_pred_sample =
					compute_high_res_pred_sample(pred_cent_local_diff, local_sum, Omega, R, Smid, Smax, Smin);

				int64_t double_res_pred_sample =
					double_resolution_predicted_sample(img, high_res_pred_sample, i, j, k, Omega, P, Smid);
				dbl_res = double_res_pred_sample;

				int64_t predicted_sample_value =
					predicted_sample(double_res_pred_sample);

				/*
				 * Quantizer
				 */
				
				int64_t prediction_residual =
					compute_prediction_residual(img[i][j][k], predicted_sample_value);

				int64_t quantizer_index =
					compute_quantizer_index(prediction_residual, i, j, k);
					
				int64_t mapped_quantizer_index =
					compute_mapped_quantizer_index(
						quantizer_index,
						double_res_pred_sample,
						predicted_sample_value,
						j, k, Smin, Smax);
						
						
				/*
				 * Result
				 */
				
				res[i][j][k] = mapped_quantizer_index;
				
				tmp[i][j][k] = quantizer_index;
			}
		}
	}

	return;
	printk("Quantizer Index\n\n");
    for (int i = 0; i < IMG_Z; ++i)
    {
        for (int j = 0; j < IMG_Y; ++j)
        {
            for (int k = 0; k < IMG_X; ++k)
				printf("%"PRId64" ", tmp[i][j][k]);
            printk("\n");
        }
		printk("\n");
    }
	printk("\n");
}

int32_t inner_product(int32_t const* v1, int32_t const* v2, uint32_t size)
{
	int32_t sum = 0;
	for (uint32_t i = 0; i < size; ++i)
		sum += v1[i] * v2[i];

	return sum;
}

int32_t compute_local_sum(int32_t img[IMG_Z][IMG_Y][IMG_X], vec3 const* N, int z, int y, int x)
{
	/*
	 * [4.4.2]
	 * Wide Neighbor
	 * Narrow Neighbor
	 * Wide Column
	 * Narrow Column
	 */

	switch (LOCAL_SUM_TYPE)
	{
	case LocalSum_WIDE_NEIGHBOR:
	{
		if (y > 0 && x > 0 && x < N->x - 1)
		{
			return img[z][y][x - 1] + img[z][y - 1][x - 1] +
				   img[z][y - 1][x] + img[z][y - 1][x + 1];
		}
		else if (y == 0 && x > 0)
		{
			return 4 * img[z][y][x];
		}
		else if (y > 0 && x == 0)
		{
			return 2 * (img[z][y - 1][x] + img[z][y - 1][x + 1]);
		}
		else if (y > 0 && x == N->x - 1)
		{
			return img[z][y][x - 1] + img[z][y - 1][x - 1] +
				   (2 * img[z][y - 1][x]);
		}
		else
		{
			// The value of local sum at t=0 is undefined since it is not
			// needed, so we return 0.
			return 0;
		}

		break;
	}
	case LocalSum_NARROW_NEIGHBOR:
	{
		if (y > 0 && 0 < x && x < N->x - 1)
		{
			return img[z][y - 1][x - 1] + 2 * img[z][y - 1][x] +
				  img[z][y - 1][x + 1];
		}
		else if (y == 0 && x > 0 && z > 0)
		{
			return 4 * img[z - 1][y][x - 1];
		}
		else if (y > 0 && x == 0)
		{
			return 2 * (img[z][y - 1][x] + img[z][y - 1][x + 1]);
		}
		else if (y > 0 && x == N->x - 1)
		{
			return 2 * (img[z][y - 1][x - 1] + 2 * img[z][y - 1][x]);
		}
		else if (y == 0 && x > 0 && z == 0)
		{
			return 4; // * kSmid;
		}
		break;
	}
	case LocalSum_WIDE_COLUMN:
	{
		if (y > 0)
			return 4 * img[z][y - 1][x];
		else if (y == 0 && x > 0)
			return 4 * img[z][y][x - 1];

		break;
	}
	case LocalSum_NARROW_COLUMN:
	{
		if (y > 0)
			return 4 * img[z][y - 1][x];
		else if (y == 0 && x > 0 && z > 0)
			return 4 * img[z - 1][y][x - 1];
		else if (y == 0 && x > 0 && z == 0)
			return 4; // * kSmid;

		break;
	}
	}

	return 0;
}

LocalDiff compute_local_diffs(int32_t img[IMG_Z][IMG_Y][IMG_X], int32_t z, int32_t y, int32_t x, uint32_t local_sum)
{
	// t=0 is not defined.
	if (x == 0 && y == 0)
		return (LocalDiff){ 0, 0, 0, 0 };
	
	LocalDiff ld;
	
	/*
	 * [4.5.1] Central Local Difference
	 */

	ld.central = (4 * img[z][y][x]) - local_sum;

	/*
	 * [4.5.2] Directional Local Differences
	 */

	if (y > 0)
		ld.north = (4 * img[z][y - 1][x]) - local_sum;
	else
		ld.north = 0;

	if (x > 0 && y > 0)
		ld.west = (4 * img[z][y][x - 1]) - local_sum;
	else if (x == 0 && y > 0)
		ld.west = (4 * img[z][y - 1][x]) - local_sum;
	else
		ld.west = 0;

	if (x > 0 && y > 0)
		ld.northwest = (4 * img[z][y - 1][x - 1]) - local_sum;
	else if (x == 0 && y > 0)
		ld.northwest = (4 * img[z][y - 1][x]) - local_sum;
	else
		ld.northwest = 0;

	return ld;
}

int32_t compute_pred_cent_local_diff(int32_t z, int32_t y, int32_t x, LocalDiff local_diffs[IMG_Z][IMG_Y][IMG_X], int32_t const* weights)
{
	if (x == 0 && y == 0)
		return 0;
	
	/*
	 * [4.5.3] Local Difference Vector
	 */

	int32_t local_diff_vec[Cz(z)];
	local_diff_vec[0] = local_diffs[z][y][x].north;
	local_diff_vec[1] = local_diffs[z][y][x].west;
	local_diff_vec[2] = local_diffs[z][y][x].northwest;

	for (int i = 1, j = 3; i <= Pz(z); ++i, ++j)
		local_diff_vec[j] = local_diffs[z - i][y][x].central;
	
	return inner_product(weights, local_diff_vec, Cz(z));
}

void initialize_weights(int32_t* weights, int z, int32_t omega)
{
	for (int i = 0; i < 3; i++)
		weights[i] = 0;
	
	int two_omega = 1 << omega;
	weights[3] = (7 * two_omega) >> 3; // 7/8 * 2^omega
	
	for (int i = 4; i < 3 + Pz(z); ++i)
		weights[i] = weights[i - 1] >> 3; // floor(1/8 * weights[i-1])
}

int32_t update_weight(int32_t omega_t, int32_t e_z_t, int32_t p_t, int32_t xi_z_i, 
	int32_t d_z_i_t1, int32_t omega_min, int32_t omega_max)
{
	// Step 1: Compute sgn[e_z(t)]
	int32_t sign = sgn_pos(e_z_t);

	// Step 2: Compute the exponent -p(t) + xi_z^(i)
	int32_t exponent = -p_t + xi_z_i;

	// Step 3: Compute 2^(-p(t) + xi_z^(i)) * d_z-i(t+1)
	// Since 2^exponent can be a left or right shift:
	int32_t scaling_factor;
	if (exponent >= 0) {
		scaling_factor = d_z_i_t1 << exponent; // 2^exponent * d_z-i(t+1)
	}
	else {
		scaling_factor = d_z_i_t1 >> (-exponent); // 2^(-|exponent|) * d_z-i(t+1)
	}

	// Step 4: Compute the update term: (1/2) * sgn[e_z(t)] * 2^(-p(t) + xi_z^(i)) * d_z-i(t+1)
	// 1/2 is a right shift by 1
	int32_t update_term = (sign * scaling_factor) >> 1;

	// Step 5: Update omega: omega(t) + floor(update_term)
	// Since we're using integers, the floor is implicit
	int32_t new_omega = omega_t + update_term;

	// Step 6: Clip the result to [omega_min, omega_max]
	new_omega = clip(new_omega, omega_min, omega_max);

	return new_omega;
}

int64_t mod(int64_t M, int64_t n)
{
	return M - n * (int64_t)(M / n);
}

int64_t compute_high_res_pred_sample(int32_t pred_cent_local_diff, int32_t local_sum, int32_t Omega, int32_t R, int32_t Smid, int32_t Smax, int32_t Smin)
{
	// The calculation shown at 4.7.2 can be represented in this format,
	//   clip(modR(c1) + c2 + c3, {clip_min, clip_max})

	uint64_t const two_R		 = (unsigned long long)1 << R;
	uint64_t const two_R_minus_1 = (unsigned long long)1 << (R - 1);
	
	int64_t const two_O 	   = (unsigned long long)1 << Omega;
	int64_t const two_O_plus_1 = (unsigned long long)1 << (Omega + 1);
	int64_t const two_O_plus_2 = (unsigned long long)1 << (Omega + 2);
	
	int64_t c1 = pred_cent_local_diff + (two_O * (local_sum - (4 * Smid)));
	int64_t modR = mod(c1 + two_R_minus_1, two_R) - two_R_minus_1;

	int64_t c2 = two_O_plus_2 * Smid;

	int64_t c3 = two_O_plus_1;

	int64_t prediction = modR + c2 + c3;

	int64_t clip_min = two_O_plus_2 * Smin;
	int64_t clip_max = two_O_plus_2 * Smax + two_O_plus_1;

	if (prediction < clip_min)
		return clip_min;

	if (prediction > clip_max)
		return clip_max;

	return prediction;
}

int64_t double_resolution_predicted_sample(int32_t img[IMG_Z][IMG_Y][IMG_X], int64_t high_res_pred_sample_val, int z, int y, int x, int32_t Omega, int32_t P, int32_t Smid)
{
	int64_t const two_O_plus_1 = 1 << (Omega + 1);

	if (x > 0 && y > 0)
		return high_res_pred_sample_val / two_O_plus_1;
	else if (x == 0 && y == 0 && P > 0 && z > 0)
		return 2 * img[z - 1][y][x];
	else
		return 2 * Smid;
}

int64_t predicted_sample(int64_t double_res_pred_sample)
{
	return (int64_t)(double_res_pred_sample / 2);
}

int64_t compute_prediction_residual(int64_t sample_value, int64_t predicted_value)
{
	return sample_value - predicted_value;
}

int64_t compute_quantizer_index(int64_t prediction_residual, int z, int y, int x)
{
	if (x == 0 && y == 0)
	{
		return prediction_residual;
	}
	else
	{
		int sgn;
		if (prediction_residual > 0)
			sgn = 1;
		else if (prediction_residual == 0)
			sgn = 0;
		else
			sgn = -1;

		return sgn * (int64_t)(abs(prediction_residual));
	}
}

int64_t compute_mapped_quantizer_index(int64_t quantizer_index, int32_t d, int64_t predicted_sample_value, int y, int x, int32_t Smin, int32_t Smax)
{
	int64_t theta = fmin(predicted_sample_value - Smin, Smax - predicted_sample_value);

	if (abs(quantizer_index) > theta)
		return abs(quantizer_index) + theta;
	else
	{
		int64_t c = pow(-1, d) * quantizer_index;
		if (c >= 0 && c <= theta)
			return 2 * abs(quantizer_index);
		else
			return 2 * abs(quantizer_index) - 1;
	}
}
