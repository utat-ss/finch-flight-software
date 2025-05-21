#include "util.h"

#include <stdint.h>

int clip(int value, int min, int max) {
    return (value < min) ? min : (value > max) ? max : value;
}

int32_t inner_product(int32_t const* v1, int32_t const* v2, uint32_t size)
{
	int32_t sum = 0;
	for (uint32_t i = 0; i < size; ++i) {
		sum += v1[i] * v2[i];
	}

	return sum;
}
