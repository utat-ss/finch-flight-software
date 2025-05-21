#pragma once

#include <stdint.h>

typedef struct {
	uint16_t z, y, x;
} vec3;

int clip(int value, int min, int max);

int32_t inner_product(
	int32_t const* v1,
	int32_t const* v2,
	uint32_t size
);
