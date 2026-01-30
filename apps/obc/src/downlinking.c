/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file downlinking.c
 * @brief Downlinking Command Sequence Implementation
 */

#include "common.h"
#include "downlinking.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);
LOG_MODULE_REGISTER(downlinking, LOG_LEVEL_INF);

/**
 * @brief Main loop for Downlinking command sequence
 */
int downlinking_command_sequence(void)
{
	cmd_adcs_mode(MODE_ADCS_FINE_POINTING, NULL, 0.0, NULL); // Placeholder values for orbit_info, current_time and tle

	downlink_type dl_type = cmd_rf_prepare_downlink();
	char* downlink_data;

	if (dl_type == DOWNLINK_TELEMETRY)
	{
		downlink_data = cmd_get_telemetry_data();
	}

	else if (dl_type == DOWNLINK_IMAGE)
	{
		downlink_data = cmd_get_image_data();
	}

	cmd_rf_downlink_data(downlink_data);

	enter_mode_op(MODE_OP_IDLE);

	return 0;
}

/**
 * @brief Prepare RF for downlinking
 */
downlink_type cmd_rf_prepare_downlink(void)
{
	// Implement RF downlink preparation
	// Return type of downlinking
	return NULL;
}

/**
 * @brief Get telemetry data for downlinking
 */
char* cmd_get_telemetry_data(void)
{
	// Get telemetry data
	return NULL;
}

/**
 * @brief Get image data for downlinking
 */
char* cmd_get_image_data(void)
{
	// Get image data
	return NULL;
}

/**
 * @brief Downlink data to RF
 */
int cmd_rf_downlink_data(char* downlink_data)
{
	// Implement downlinking data to RF
	return 0;
}