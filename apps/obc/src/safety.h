/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file safety.h
 * @brief Header for Safety Command Sequence Implementation
 */

#ifndef SAFETY_H
#define SAFETY_H

#include "common.h"

/**
 * @brief Safety commands
 */
typedef enum safety_command {
	CMD_SAFETY_EXAMPLE
} safety_command;

/**
 * @brief Main loop for Safety command sequence
 */
int safety_command_sequence(void);

/**
 * @brief Get error info from RF
 */
char* cmd_rf_get_error_info(void);

/**
 * @brief Get error handler from RF
 */
safety_command cmd_rf_get_error_handler(void);

/**
 * @brief Send error info to RF
 */
int cmd_rf_send_error_info(char* error_info);

/**
 * @brief Send error handler command to RF
 */
int cmd_rf_execute_error_handler(safety_command cmd);

/**
 * @brief Check if operator commands to exit Safety mode
 */
int cmd_rf_check_exit_safety_mode(void);

#endif