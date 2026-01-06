/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>

#include <zephyr/logging/log.h>
#include <zephyr/ztest.h>

LOG_MODULE_REGISTER(bench);

ZTEST_SUITE(is_alive_test, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Tests ID of ADCS dev board matches data sheet.
 */
ZTEST(is_alive_test, test_get_id)
{
	adcs_id_t adcs_id_received;
	adcs_id_t adcs_id_expected = 0x54;
	adcs_get_id_ret ret;

	ret = adcs_get_id(adcs_id_received);
	zassert_equal(ret, ADCS_GET_ID_RET_ERR,
		"Failed to read ADCS ID with error %d", (int)ret);
	zassert_equal(adcs_id_received, adcs_id_expected,
		"Received ADCS ID %d does not match expected ID %d",
		adcs_id_expected, adcs_id_received);
}
