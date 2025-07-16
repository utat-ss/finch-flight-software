/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);

int main(void)
{
	while (1) {
		LOG_INF("obc");
    printk("obc wassup\n");
		k_msleep(1000);
	}
	return 0;
}
