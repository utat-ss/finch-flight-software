/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/ccsds123b/arrays.h>
#include <finch/ccsds123b/predictor.h>

void update_local_diffs(LocalDiffs diffs, int z, int y, int x, const LocalDiff *l)
{
	diffs[z][y][x] = *l;
}

LocalDiff get_local_diffs(const LocalDiffs diffs, int z, int y, int x)
{
	return diffs[z][y][x];
}

void update_predictions(Predictions p, int z, int y, int x, int32_t value)
{
	p[z][y][x] = value;
}

int32_t get_predictions(const Predictions p, int z, int y, int x)
{
	return p[z][y][x];
}

EncoderOutItem get_encoder_out(EncoderOut out, int z, int y, int x)
{
	return out[z][y][x];
}
