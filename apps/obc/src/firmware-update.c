/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file firmware-update.c
 * @brief Firmware Update Command Sequence Implementation
 */

#include "common.h"
#include "firmware-update.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);
LOG_MODULE_REGISTER(firmware_update, LOG_LEVEL_INF);

/**
 * @brief Main loop for Firmware Update command sequence
 */
int firmware_update_command_sequence(void)
{
	// RUN BELOW IN PARALLEL

	// Parallel block 1
	cmd_adcs_mode(MODE_ADCS_SUN_POINTING, NULL, 0.0, NULL); // Placeholder values for orbit_info, current_time and tle

	// Parallel block 2
	if (cmd_obc_available_update())
	{
		cmd_obc_update();
		// Transmit update feedback to RF
	}

	else if (cmd_pay_available_update())
	{
		cmd_pay_update();
		// Transmit update feedback to RF
	}

	// END PARALLEL

	enter_mode_op(MODE_OP_IDLE);

	return 0;
}

/**
 * @brief Polls for available OBC firmware upate
 */
int cmd_obc_available_update(void)
{
	// Implement check for OBC firmware update
	return 0;
}

/**
 * @brief Polls for available PAY firmware upate
 */
int cmd_pay_available_update(void)
{
	// Implement check for PAY firmware update
	return 0;
}

/**
 * @brief Executes OBC firmware update
 */
int cmd_obc_update(void)
{
	// Implement OBC firmware update
	return 0;
}

/**
 * @brief Executes PAY firmware update
 */
int cmd_pay_update(void)
{
	// Implement PAY firmware update
	return 0;
}