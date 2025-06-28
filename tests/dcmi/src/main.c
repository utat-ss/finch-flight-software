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
#include <zephyr/drivers/dma/dma_stm32.h>
#include "dcmi/dcmi_ov7670.h"

struct video_format fmt = {
    .pixelformat = VIDEO_PIX_FMT_RGB565,
    .width = 320,
    .height = 240,
};

const struct device* ov7670 = DEVICE_DT_GET(DT_NODELABEL(ov7670));

LOG_MODULE_REGISTER(pay);

int main(void)
{
    if (camera_init(ov7670, &fmt)) {
        LOG_ERR("Failed to start camera");
        return 1;
    }
    camera_loop(ov7670);

	return 0;
}
