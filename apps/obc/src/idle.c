/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file idle.c
 * @brief Idle Command Sequence Implementation
 */

#include "common.h"
#include "idle.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(idle);

/**
 * @brief Main loop for Idle command sequence
 */
int idle_command_sequence(void)
{
	idle_command rf_command;
	mode_op rf_mode_change;

	idle_command scheduled_cmd;
	mode_op scheduled_mode;

	// Enter schedule based on location

	cmd_adcs_mode(MODE_ADCS_SUN_POINTING, 0, 0.0, 0); // Placeholder values for orbit_info, current_time and tle

	// RUN BELOW IN PARALLEL
	
	// Parallel block 1
	rf_command = rf_incoming_command();
	rf_mode_change = rf_incoming_mode_change();

	// If command from RF
	if (rf_command != NULL)
	{
		schedule_command(rf_command);
	}
	
	// If mode change from RF
	else if (rf_mode_change != NULL)
	{
		schedule_mode_change(rf_mode_change);
	}

	// Parallel block 2

	while (1)
	{
		// Wait for ping

		scheduled_cmd = check_scheduled_command();
		scheduled_mode = check_scheduled_mode_change();

		if (scheduled_cmd != NULL)
		{
			execute_command(scheduled_cmd);
		}

		else if (scheduled_mode != NULL)
		{
			enter_mode_op(scheduled_mode);
		}
	}

	// END PARALLEL

	return 0;
}

/**
 * @brief Check RF for incoming command
 */
idle_command rf_incoming_command(void)
{
	// Check RF for incoming command
	return NULL;
}

/**
 * @brief Check RF for incoming mode change
 */
mode_op rf_incoming_mode_change(void)
{
	// Check RF for incoming mode change
	return NULL;
}

/**
 * @brief Schedule specified command
 */
int schedule_command(idle_command cmd)
{
	// Schedule specified command
	return 0;
}

/**
 * @brief Schedule specified mode change
 */
int schedule_mode_change(mode_op mode)
{
	// Schedule specified mode change
	return 0;
}

/**
 * @brief Check if any command is scheduled
 */
idle_command check_scheduled_command(void)
{
	// Check if any command is scheduled
	return NULL;
}

/**
 * @brief Check if any mode change is scheduled
 */
mode_op check_scheduled_mode_change(void)
{
	// Check if any mode change is scheduled
	return NULL;
}

/**
 * @brief Execute specified command
 */
int execute_command(idle_command cmd)
{
	// Execute specified command
	return 0;
}