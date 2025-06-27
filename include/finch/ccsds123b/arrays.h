#ifndef FINCH_ARRAYS_H
#define FINCH_ARRAYS_H

#include <stdint.h>

typedef struct {
	int32_t central;
	int32_t north;
	int32_t west;
	int32_t northwest;
} LocalDiff;

typedef LocalDiff LocalDiffs[10][10][10];

void update_local_diffs(LocalDiffs local_diffs, int z, int y, int x, const LocalDiff * l);
LocalDiff get_local_diffs(const LocalDiffs local_diffs, int z, int y, int x);

typedef int32_t Predictions[10][10][10];

void update_predictions(Predictions predictions, int z, int y, int x, int32_t p);
int32_t get_predictions(const Predictions predictions, int z, int y, int x);

#endif