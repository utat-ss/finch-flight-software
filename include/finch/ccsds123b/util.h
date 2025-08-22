/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_UTIL_H
#define FINCH_INCLUDE_UTIL_H

#include <stdint.h>

typedef struct {
	uint16_t z, y, x;
} vec3;

int64_t min(int64_t a, int64_t b);

int clip(int value, int min, int max);

int32_t inner_product(
	const int32_t *v1,
	const int32_t *v2,
	uint32_t size
);

#endif /* FINCH_INCLUDE_UTIL_H */
