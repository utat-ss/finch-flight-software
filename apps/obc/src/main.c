/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#include <finch/adcs/adcs.h>

LOG_MODULE_REGISTER(obc);

void LOG_ARR(uint8_t *arr, uint32_t len)
{
	for (int32_t i = 0; i < len; ++i) {
		LOG_RAW("0x%02X ", arr[i]);
	}
	LOG_RAW("\n");
}

int main(void)
{
    int ret;
	uint8_t adcs_snid[17];

	for (int i = 0; i < 17; ++i)
		adcs_snid[i] = 127;

    ret = finch_adcs_init();
    if (ret != FINCH_ADCS_RET_OK) {
        LOG_INF("finch_adcs_init failed with %d", ret);
        return 0;
    }

	ret = finch_adcs_register_read(FINCH_ADCS_REGISTER_SNID, adcs_snid, sizeof(adcs_snid));
	if (ret != FINCH_ADCS_RET_OK) {
		LOG_INF("finch_adcs_register_read failed with %d", ret);
		return 0;
	}

	LOG_RAW("ADCS SNID: ");
	LOG_ARR(adcs_snid, sizeof(adcs_snid));

	while (1) {
		k_msleep(1000);
	}

	return 0;
}
