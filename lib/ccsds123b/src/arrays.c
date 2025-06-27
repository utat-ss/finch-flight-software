#include <finch/ccsds123b/arrays.h>
#include <finch/ccsds123b/predictor.h>

void update_local_diffs(LocalDiffs local_diffs, int z, int y, int x, const LocalDiff * l) {
    local_diffs[z][y][x] = *l;
}

LocalDiff get_local_diffs(LocalDiffs local_diffs, int z, int y, int x) {
    return local_diffs[z][y][x];
}

void update_predictions(Predictions predictions, int z, int y, int x, int32_t p) {
    predictions[z][y][x] = p;
}

int32_t get_predictions(const Predictions predictions, int z, int y, int x) {
    return predictions[z][y][x];
}