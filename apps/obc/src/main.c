/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>
#include <finch/csp/csp.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(obc);

int main(void)
{
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

	LOG_RAW("ADCS ID: ");
	for (uint8_t i = 0; i < ADCS_ID_SIZE; ++i) {
		LOG_RAW("0x%02x ", adcs_id[i]);
	}
	LOG_RAW("\n");

	while (1) {
		k_msleep(1000);
	}

	return 0;
}
