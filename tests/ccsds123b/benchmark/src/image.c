/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/ccsds123b/image.h>
#include <finch/ccsds123b/constants.h>

#include <finch/ccsds123b/util.h>

vec3 N_tile = (vec3){ N_TILE_Z, N_TILE_Y, N_TILE_X };
int img[N_TILE_Z][N_TILE_Y][N_TILE_X];

vec3 N = (vec3) { N_Z, N_Y, N_X };

void img_init(void)
{
	int c = 0;

	for (int z = 0; z < N_tile.z; ++z) {
		for (int y = 0; y < N_tile.y; ++y) {
			for (int x = 0; x < N_tile.x; ++x) {
				img[z][y][x] = c++;
			}
		}
	}
}

const vec3 *img_get_N(void)
{
	return &N;
}

int img_get_pxl(int z, int y, int x)
{
	return img[z % N_tile.z][y % N_tile.y][x % N_tile.x];
}

int img_get_pxl_t(int z, int t)
{
	int y = t / N.x;
	int x = t % N.x;

	return img_get_pxl(z, y, x);
}
