/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/csp/csp.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);

int main(void)
{
	int ret = finch_csp_init();

	if (ret < 0) {
		/* TODO FDIR must handle this situation! */
		LOG_ERR("Failed to initialize FINCH CSP (%d)", ret);
		return ret;
	}

	while (1) {
		LOG_INF("obc");
		k_msleep(1000);
	}
	return 0;
}
