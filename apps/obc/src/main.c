/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>
#include <finch/csp/csp.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>
#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/kernel.h>

#define CAN_ID_ADCS 0x10
#define ADCS_CMD_GET_ID 0x01

LOG_MODULE_REGISTER(obc);

const struct device *const can = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

struct adcs_request {
	uint8_t cmd;
};

struct k_thread adcs_thread_data;

K_THREAD_STACK_DEFINE(adcs_stack, 2048);
K_MSGQ_DEFINE(adcs_req_q, sizeof(struct adcs_request), 8, 4);

void rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
	LOG_INF("CAN CMD ID[0x%x]", frame->id);

	const int dlc = frame->dlc;

	if (dlc != 1) {
		LOG_ERR("Commands are one byte sized");
		return ;
	}

	struct adcs_request req = {
		.cmd = frame->data[0],
	};

	int ret = k_msgq_put(&adcs_req_q, &req, K_NO_WAIT);

	if (ret != 0) {
		LOG_ERR("MSGQ FULL OR FAIL (%d)", ret);
	} else {
		LOG_INF("ADCS QUEUED REQ");
	}
}

int can_init()
{
	int ret;

	if (IS_ENABLED(CONFIG_CAN_MODE_LOOPBACK)) {
		ret = can_set_mode(can, CAN_MODE_LOOPBACK);
		if (ret != 0) {
			LOG_ERR("Error setting loopback mode [%d]", ret);
			return 1;
		}
		LOG_INF("Loopback mode enabled");
	}

	ret = can_start(can);

	const struct can_filter pi_filter = {
		.flags = 0U,
		.id = CAN_ID_ADCS,
		.mask = CAN_STD_ID_MASK
	};

	can_add_rx_filter(can, &rx_callback, NULL, &pi_filter);

	return 0;
}

void send_can_response(uint8_t id[ADCS_ID_SIZE])
{
	int ret;

	// frame 1 (8 bytes) 
	struct can_frame frame1 = {
		.flags = 0U,
		.id = 0x01,
		.dlc = 8,
	};

	memcpy(frame1.data, &id[0], 8);

	// frame 2 (4 bytes)
	struct can_frame frame2 = {
		.flags = 0U,
		.id = 0x02,
		.dlc = 4,
	};

	memcpy(frame2.data, &id[8], 4);

	ret = can_send(can, &frame1, K_MSEC(100), NULL, NULL);
	if (ret != 0) {
		LOG_ERR("x CAN send frame1 failed (%d)", ret);
	}

	ret = can_send(can, &frame2, K_MSEC(100), NULL, NULL);
	if (ret != 0) {
		LOG_ERR("x CAN send frame2 failed (%d)", ret);
	}

	LOG_INF("ADCS ID SENT OVER CAN");
}

void adcs_thread(void *a, void *b, void *c)
{
	struct adcs_request req;

	while (1) {
		k_msgq_get(&adcs_req_q, &req, K_FOREVER);

		switch (req.cmd) {
			case ADCS_CMD_GET_ID: {
				LOG_INF("ADCS THREAD CMD[0x%x]", req.cmd);

				uint8_t id[ADCS_ID_SIZE];

				adcs_rc_t rc = adcs_get_id(id, ADCS_ID_SIZE);
				if (rc != ADCS_RC_OK) {
					LOG_ERR("ADCS failed");
					continue;
				}

				LOG_INF("ADCS GET ID");

				send_can_response(id);
				break;
			}

			default:
				LOG_WRN("UNKNOWN ADCS CMD 0x%x", req.cmd);
				break;
		}
		// potential to add more request types: adcs_get..., a single
		// adcs thread handles all accesses to adcs component
	}
}

int main(void)
{
	int can_rc;
	adcs_rc_t adcs_rc;

	LOG_INF("Board Started");

	/*
	 *	Get internal watchdog from device tree. watchdog0 holdes the address to iwdg, which defines the device in the device tree
	 */
	const struct device *wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));
	int wdt_channel_id;

	if (!device_is_ready(wdt)) {
		LOG_ERR("%s: device is not ready", wdt->name);
		return 0;
	}

	struct wdt_timeout_cfg wdt_config = {
		/* Reset SoC when watchdog downcounter reaches 0. */
		.flags = WDT_FLAG_RESET_SOC,

		/* Set min and max time window for feeding the timer */
		.window.min = CONFIG_WDT_MIN_WINDOW,
		.window.max = CONFIG_WDT_MAX_WINDOW,
	};

	wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);

	/* Timeout configuration setup */

	if (wdt_channel_id < 0) {
		LOG_ERR("Internal watchdog install error");
		return 0;
	}

	int err = wdt_setup(wdt, CONFIG_WDT_OPT);

	if (err < 0) {
		LOG_ERR("Internal watchdog setup error");
		return 0;
	}

	LOG_INF("CAN START...");

	can_rc = can_init();
	if (can_rc != 0) {
		LOG_ERR("Failed to initialize CAN (%d)", can_rc);
		return can_rc;
	}

	LOG_INF("CAN READY");

	LOG_INF("ADCS START...");

	adcs_rc = adcs_init();
	if (adcs_rc != ADCS_RC_OK) {
		LOG_ERR("Failed to initialize ADCS (%d)", adcs_rc);
		return adcs_rc;
	}

	LOG_INF("ADCS READY");

	LOG_INF("ADCS THREAD INIT");
	k_thread_create(&adcs_thread_data,
				adcs_stack,
				K_THREAD_STACK_SIZEOF(adcs_stack),
				adcs_thread,
				NULL, NULL, NULL,
				5, 0, K_NO_WAIT);

	while (1) {

		/*
		 *	Main loop runs here. If it hangs, feeding will not run
		 */

		int ret = wdt_feed(wdt, wdt_channel_id);

		if (ret < 0) {
			LOG_ERR("Failed to feed the watchdog");
			return 0;
		}
		k_msleep(CONFIG_WDT_CHECK_INTERVAL);
	}

	return 0;
}
