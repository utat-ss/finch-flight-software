#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/video.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>

/**
 * @brief INIT.
 *
 * Insert Description
 *
 * @param cam Pointer to the device structure for the driver instance.
 * @param fmt Pointer to a video format struct.
 *
 * @retval 0 Is successful.
 * @retval -1 Failed to initiate
 */
int camera_init(const struct device* cam, struct video_format* fmt);

/**
 * @brief LOOP.
 *
 * Insert Description
 *
 * @param cam Pointer to the device structure for the driver instance.
 * @retval None
 */
void camera_loop(const struct device* cam);


