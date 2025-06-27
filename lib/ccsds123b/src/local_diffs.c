#include <finch/ccsds123b/local_diffs.h>

LocalDiffs local_diffs;

void update_local_diffs(int z, int y, int x, const LocalDiff* l) {
    local_diffs[z][y][x] = *l;
}

LocalDiff get_local_diffs(int z, int y, int x) {
    return local_diffs[z][y][x];
}
