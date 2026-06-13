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

	LOG_INF("ADCS ID: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
			adcs_id[0], adcs_id[1], adcs_id[2], adcs_id[3], adcs_id[4], adcs_id[5],
			adcs_id[6], adcs_id[7], adcs_id[8], adcs_id[9], adcs_id[10], adcs_id[11]);
	
	uint8_t adcs_error[ADCS_ERROR_SIZE];
	LOG_INF("ADCS Error: 0x%x 0x%x 0x%x 0x%x", adcs_error[0], acds_error[1], acds_error[2], acds_error[3]);
	
	while (1) {
		k_msleep(1000);
	}

	return 0;
}
