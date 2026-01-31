/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file common.c
 * @brief Source for Definitions Common Across Modules 
 */

#include "common.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(finch_common);

/**
 * @brief Enter specified mode of operation
 */
void enter_mode_op(mode_op mode)
{
	switch (mode) {
		case MODE_OP_IDLE:
			LOG_INF("Entering Idle mode");
			// Add implementation for entering Idle mode
			break;

		case MODE_OP_IMAGING:
			LOG_INF("Entering Imaging mode");
			// Add implementation for entering Imaging mode
			break;

		case MODE_OP_DOWNLINKING:
			LOG_INF("Entering Downlinking mode");
			// Add implementation for entering Downlinking mode
			break;

		case MODE_OP_SAFETY:
			LOG_INF("Entering Safety mode");
			// Add implementation for entering Safety mode
			break;

		case MODE_OP_FIRMWARE_UPDATE:
			LOG_INF("Entering Firmware Update mode");
			// Add implementation for entering Firmware Update mode
			break;

		default:
			LOG_WRN("Unknown mode of operation");
			break;
	}
}

/**
 * @brief Enter specified ADCS control mode
 */
void cmd_adcs_mode(mode_adcs mode, uint8_t* orbit_info, float current_time, uint8_t* tle)
{
	switch (mode) {
		case MODE_ADCS_SAFE:
			LOG_INF("Setting ADCS to Safe mode");
			// Setting ADCS to Safe mode
			break;

		case MODE_ADCS_DETUMBLING:
			LOG_INF("Setting ADCS to Detumbling mode");
			// Set ADCS to Detumbling mode
			break;

		case MODE_ADCS_SUN_POINTING:
			LOG_INF("Setting ADCS to Sun Pointing mode");
			// Set ADCS to Sun Pointing mode
			break;

		case MODE_ADCS_FINE_POINTING:
			LOG_INF("Setting ADCS to Fine Pointing mode");
			// Set ADCS to Fine Pointing mode
			break;

		case MODE_ADCS_LVLH:
			LOG_INF("Setting ADCS to LVLH mode");
			// Set ADCS to LVLH mode
			break;

		case MODE_ADCS_TRACKING:
			LOG_INF("Setting ADCS to Tracking mode");
			// Set ADCS to Tracking mode
			break;

		case MODE_ADCS_OFF:
			LOG_INF("Setting ADCS to Off mode");
			// Set ADCS to Off mode
			break;

		default:
			LOG_WRN("Unknown ADCS control mode");
			break;
	}
}

/**
 * @brief Enter specified PAY control mode
 */
void cmd_pay_mode(mode_pay mode)
{
	switch (mode) {
		case MODE_PAY_OFF:
			LOG_INF("Setting PAY to Off mode");
			// Set PAY to Off mode
			break;

		case MODE_PAY_ON:
			LOG_INF("Setting PAY to On mode");
			// Set PAY to On mode
			break;

		default:
			LOG_WRN("Unknown PAY control mode");
			break;
	}
}