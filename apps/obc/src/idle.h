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

/**
 * @brief Idle commands
 */
typedef enum idle_command {
	CMD_IDLE_EXAMPLE
} idle_command;

/**
 * @brief Main loop for Idle command sequence
 */
int idle_command_sequence(void);

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
int execute_command(idle_command cmd);

#endif