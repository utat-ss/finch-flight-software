#ifndef LOCAL_DIFFS_H
#define LOCAL_DIFFS_H

#include <stdint.h>

typedef struct {
	int32_t central;
	int32_t north;
	int32_t west;
	int32_t northwest;
} LocalDiff;

typedef LocalDiff LocalDiffs[10][10][10];

void update_local_diffs(int z, int y, int x, const LocalDiff* l);
LocalDiff get_local_diffs(int z, int y, int x);

#endif