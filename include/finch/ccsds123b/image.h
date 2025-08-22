/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_INCLUDE_IMAGE_H
#define FINCH_INCLUDE_IMAGE_H

#include <finch/ccsds123b/util.h>

void img_init(void);

const vec3 *img_get_N(void);

int img_get_pxl(int z, int y, int x);

int img_get_pxl_t(int z, int t);

#endif /* FINCH_INCLUDE_IMAGE_H */
