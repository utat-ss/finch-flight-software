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

LOG_MODULE_REGISTER(downlinking);

/**
 * @brief Main loop for Downlinking command sequence
 */
int downlinking_command_sequence(void)
{
	cmd_adcs_mode(MODE_ADCS_FINE_POINTING, 0, 0.0, 0); // Placeholder values for orbit_info, current_time and tle

	downlink_type dl_type = downlinking_prepare_rf();
	uint8_t* downlink_data;

	if (dl_type == DOWNLINK_TELEMETRY)
	{
		downlink_data = downlinking_get_telemetry_data();
	}

	else if (dl_type == DOWNLINK_IMAGE)
	{
		downlink_data = downlinking_get_image_data();
	}

	int ret = downlinking_downlink_rf(downlink_data);

	// Error handling - to be implemented
	if (ret < 0)
	{
		return ret;
	}

	enter_mode_op(MODE_OP_IDLE);

	return 0;
}

/**
 * @brief Prepare RF for downlinking
 */
downlink_type downlinking_prepare_rf(void)
{
	// Implement RF downlink preparation
	// Return type of downlinking
	return NULL;
}

/**
 * @brief Get telemetry data for downlinking
 */
uint8_t* downlinking_get_telemetry_data(void)
{
	// Get telemetry data
	return NULL;
}

/**
 * @brief Get image data for downlinking
 */
uint8_t* downlinking_get_image_data(void)
{
	// Get image data
	return NULL;
}

/**
 * @brief Downlink data to RF
 */
int downlinking_downlink_rf(const uint8_t* downlink_data)
{
	// Implement downlinking data to RF
	return 0;
}