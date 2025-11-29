/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);

const struct device *const can = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

int main(void)
{
	int ret;
	struct can_frame frame = {
		.flags = 0,
		.id = 0x123,
		.dlc = 8,
		.data = { 1, 2, 3, 4, 5, 6, 7, 8 }
	};

	LOG_INF("Starting CAN send");
	
	LOG_INF("DEVICE IS READY: %d", device_is_ready(can));

	ret = can_start(can);

	LOG_INF("CAN STARTED: %d", ret);

	/* 3000ms timeout */
	while (true) {
		k_msleep(5000);
		ret = can_send(can, &frame, K_MSEC(3000), NULL, NULL);
		if (ret) {
			LOG_ERR("Sending failed [%d]", ret);
			continue;
		}

		LOG_INF("CAN sent successfully");
		break;
	}

	return 0;
}
