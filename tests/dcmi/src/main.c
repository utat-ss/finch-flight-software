/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/video.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include "include/dcmi/dcmi_ov2640.h"

struct video_format fmt = {
    .pixelformat = VIDEO_PIX_FMT_RGB565,
    .width = 320,
    .height = 240,
};

const struct device* ov2640 = DEVICE_DT_GET(DT_NODELABEL(ov2640));

LOG_MODULE_REGISTER(pay);

int main(void)
{
    if (camera_init(ov2640, &fmt)) {
        LOG_ERR("Failed to start camera");
        return 1;
    }
    camera_loop(ov2640);

	return 0;
}
