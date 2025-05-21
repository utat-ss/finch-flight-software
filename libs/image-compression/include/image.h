#pragma once

#include "util.h"

void img_init();

const vec3 *img_get_N();

int img_get_pxl(int z, int y, int x);

int img_get_pxl_t(int z, int t);
