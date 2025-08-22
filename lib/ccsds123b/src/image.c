/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/ccsds123b/image.h>

vec3 N = (vec3){ 3, 4, 4 };
int img[3][4][4];

void img_init(void)
{
	int c = 0;

	for (int z = 0; z < N.z; ++z) {
		for (int y = 0; y < N.y; ++y) {
			for (int x = 0; x < N.x; ++x) {
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
	return img[z][y][x];
}

int img_get_pxl_t(int z, int t)
{
	int y = t / N.x;
	int x = t % N.x;

	return img_get_pxl(z, y, x);
}
