/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file idle.h
 * @brief Header for Idle Command Sequence Implementation
 */

#ifndef IDLE_H
#define IDLE_H

#include "common.h"

typedef enum {
	IDLE_RET_SUCCESS,
	IDLE_ADCS_ERROR,
	// ...
	IDLE_RET_ERROR
} idle_ret_t;

/**
 * @brief Idle commands
 */
typedef struct idle_command {
	// idle_ret_t (*fname)(int argc, char *argv),

	mode_op mode;
	idle_ret_t (*fname)(void);
	uint8_t *loc;
} command_t;

command_t idle_schedule[MAX_SCHEDULE_LENGTH];

// command_t idle_schedule[MAX_SCHEDULE_LENGTH] = {
//     idle, idle_adcs 
//     idle, idle_listen
//     ...
// 	   imaging, null,
// 	   null,
// };


/**
 * @brief Main loop for Idle command sequence
 */
mode_op idle_command_sequence(void);

/**
 * @brief Check RF for incoming command
 */
idle_command rf_incoming_command(void);

/**
 * @brief Check RF for incoming mode change
 */
mode_op rf_incoming_mode_change(void);

/**
 * @brief Schedule specified command
 */
int schedule_command(idle_command cmd);

/**
 * @brief Schedule specified mode change
 */
int schedule_mode_change(mode_op mode);

/**
 * @brief Check if any command is scheduled
 */
idle_command check_scheduled_command(void);

/**
 * @brief Check if any mode change is scheduled
 */
mode_op check_scheduled_mode_change(void);

/**
 * @brief Execute specified command
 */
idle_ret_t execute_command(command_t cmd)

#endif