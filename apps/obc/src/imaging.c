/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file imaging.c
 * @brief Imaging Command Sequence Implementation
 */

#include "common.h"
#include "imaging.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);
LOG_MODULE_REGISTER(imaging, LOG_LEVEL_INF);

/**
 * @brief Main loop for Imaging command sequence
 */
int imaging_command_sequence(void)
{
	cmd_pay_camera_cool();

	cmd_adcs_mode(MODE_ADCS_FINE_POINTING, 0, 0.0, 0); // Placeholder values for orbit_info, current_time and tle

	cmd_pay_camera_image();
	LOG_INF("Image capture complete");

	// If near ground station
	if (true) // Placeholder condition
	{
		enter_mode_op(MODE_OP_DOWNLINKING);
	}

	else
	{
		LOG_INF("Downlink request"); // Log downlink request
		enter_mode_op(MODE_OP_IDLE);
	}
}

/**
 * @brief Execute camera cooling
 */
int cmd_pay_camera_cool(void)
{
	// Implement camera cooling
	return 0;
}

/**
 * @brief Execute image capture
 */
int cmd_pay_camera_image(void)
{
	// Implement image capture
	return 0;
}