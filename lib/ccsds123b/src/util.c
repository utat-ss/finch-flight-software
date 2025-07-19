#include <finch/ccsds123b/util.h>

#include <stdint.h>

int64_t min(int64_t a, int64_t b)
{
	return (a < b) ? a : b;
}

int clip(int value, int min, int max)
{
	return (value < min) ? min : (value > max) ? max : value;
}

int32_t inner_product(const int32_t *v1, const int32_t *v2, uint32_t size)
{
	int32_t sum = 0;

	for (uint32_t i = 0; i < size; ++i) {
		sum += v1[i] * v2[i];
	}

	return sum;
}
