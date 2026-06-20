/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>
#include <finch/csp/csp.h>

#include <zephyr/logging/log.h>
#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(obc);

int main(void)
{
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

	int rc;
	adcs_rc_t adcs_rc;

	rc = finch_csp_init();
	if (rc < 0) {
		LOG_ERR("Failed to initialize FINCH CSP (%d)", rc);
		return rc;
	}

	adcs_rc = adcs_init();
	if (adcs_rc != ADCS_RC_OK) {
		LOG_ERR("Failed to initialize ADCS (%d)", adcs_rc);
		return adcs_rc;
	}

	/* Verify ADCS ID. */

	uint8_t adcs_id[ADCS_ID_SIZE];
	adcs_rc = adcs_get_id(adcs_id, ADCS_ID_SIZE);

	LOG_INF("ADCS ID: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
			adcs_id[0], adcs_id[1], adcs_id[2], adcs_id[3], adcs_id[4], adcs_id[5],
			adcs_id[6], adcs_id[7], adcs_id[8], adcs_id[9], adcs_id[10], adcs_id[11]);

	uint32_t stat;
	adcs_rc = adcs_get_status(&stat); //get status of the adcs
	if (adcs_rc != ADCS_RC_OK) {
	    LOG_ERR("Failed to get ADCS status (%d)", adcs_rc);
	} else {
	    LOG_INF("ADCS STATUS: 0x%08x", stat); //print status; should be 0 (safe mode)
	    LOG_INF("Current mode: %u", stat & 0x07); /* bottom 3 bits = MODE */
	}

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
