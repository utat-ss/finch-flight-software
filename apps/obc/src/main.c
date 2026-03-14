/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>
#include <finch/csp/csp.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);

int main(void)
{
	int ret = finch_csp_init();

	if (ret < 0) {
		LOG_ERR("Failed to initialize FINCH CSP (%d)", ret);
		return ret;
	}

    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready");
        return -ENODEV;
    }

    can_start(can_dev);

    const struct can_filter pi_filter = {
        .flags = 0U,
        .id = CAN_OBC_ID,
        .mask = CAN_STD_ID_MASK
    };

    can_add_rx_filter(can_dev, pi_rx_callback, NULL, &pi_filter);

    while (1) {
        if (!k_sem_take(&rx_sem, K_FOREVER)) {
            handle_can_command(&rx_frame_storage);
        }
    }

    return 0;
}
