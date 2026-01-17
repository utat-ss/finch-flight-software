/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(flatsat);

const struct device *const can = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

void pi_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
	LOG_INF("RECEIVED CAN MESSAGE WITH ID [%x]", frame->id);

	const int dlc = frame->dlc;

	for (int i = 0; i < dlc; i++) {
		LOG_INF("%x", frame->data[i]);
	}
}

int main(void)
{
	int ret;

	LOG_INF("Starting CAN send");

	LOG_INF("DEVICE IS READY: %d", device_is_ready(can));

	// use loopback mode (send to itself) for debugging
	/*
	 * ret = can_set_mode(can, CAN_MODE_LOOPBACK);
	 * if (ret != 0) {
	 *	LOG_ERR("Error setting loopback mode [%d]", ret);
	 *	return 0;
	 * }
	 */

	ret = can_start(can);

	LOG_INF("CAN STARTED: %d", ret);

	// what to filter for regarding receiving messages
	const struct can_filter pi_filter = {
		.flags = 0U,
		.id = 0x100, // when receiving messages from the pi this must match (e.g. to send 100#ABCD .id needs to be 0x100)
		.mask = CAN_STD_ID_MASK
	};

	can_add_rx_filter(can, &pi_rx_callback, NULL, &pi_filter);

	while (true) {
		struct can_frame frame = {
			.flags = 0,
			.id = 0x123,
			.dlc = 4, // data length. Must match the number of elements in .data
			.data = { 0xde, 0xad, 0xbe, 0xef }
		};
		LOG_INF("Waiting 5 seconds");
		k_msleep(5000);
		ret = can_send(can, &frame, K_MSEC(3000), NULL, NULL);
		if (ret) {
			LOG_ERR("Sending failed [%d]", ret);
			continue;
		}

		LOG_INF("CAN sent successfully");
	}

	return 0;
}
