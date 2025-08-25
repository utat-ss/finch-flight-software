/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_ARRAYS_H
#define FINCH_ARRAYS_H

#include <finch/ccsds123b/constants.h>

#include <stdint.h>

typedef struct {
	int32_t central;
	int32_t north;
	int32_t west;
	int32_t northwest;
} LocalDiff;

typedef LocalDiff LocalDiffs[N_TILE_Z][N_TILE_Y][N_TILE_X];
typedef int32_t Predictions[N_TILE_Z][N_TILE_Y][N_TILE_X];
typedef char EncoderOut[N_TILE_Z][N_TILE_Y][N_TILE_X][32];
typedef char *EncoderOutItem;

void update_local_diffs(LocalDiffs diffs, int z, int y, int x, const LocalDiff *l);
LocalDiff get_local_diffs(const LocalDiffs diffs, int z, int y, int x);

void update_predictions(Predictions p, int z, int y, int x, int32_t value);
int32_t get_predictions(const Predictions p, int z, int y, int x);

EncoderOutItem get_encoder_out(EncoderOut out, int z, int y, int x);

#endif
