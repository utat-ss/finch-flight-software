#include <finch/ccsds123b/arrays.h>
#include <finch/ccsds123b/constants.h>

void update_local_diffs(LocalDiffs local_diffs, int z, int y, int x, const LocalDiff* l) {
    local_diffs[z % N_TILE_Z][y % N_TILE_Y][x % N_TILE_X] = *l;
}

LocalDiff get_local_diffs(const LocalDiffs local_diffs, int z, int y, int x) {
    return local_diffs[z % N_TILE_Z][y % N_TILE_Y][x % N_TILE_X];
}

void update_predictions(Predictions predictions, int z, int y, int x, int32_t p) {
    predictions[z % N_TILE_Z][y % N_TILE_Y][x % N_TILE_X] = p;
}

int32_t get_predictions(const Predictions predictions, int z, int y, int x) {
    return predictions[z % N_TILE_Z][y % N_TILE_Y][x % N_TILE_X];
}