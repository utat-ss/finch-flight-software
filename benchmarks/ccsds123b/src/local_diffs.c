#include <finch/ccsds123b/local_diffs.h>
#include "benchmark.h"

LocalDiffs local_diffs;

void update_local_diffs(int z, int y, int x, const LocalDiff* l) {
    local_diffs[z % N_TILE_Z][y % N_TILE_Y][x % N_TILE_X] = *l;
}

LocalDiff get_local_diffs(int z, int y, int x) {
    return local_diffs[z % N_TILE_Z][y % N_TILE_Y][x % N_TILE_X];
}