/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file firmware-update.h
 * @brief Header for Firmware Update Command Sequence Implementation
 */

#ifndef FIRMWARE_UPDATE_H
#define FIRMWARE_UPDATE_H

#include "common.h"

/**
 * @brief Main loop for Firmware Update command sequence
 */
int firmware_update_command_sequence(void);

/**
 * @brief Polls for available OBC firmware upate
 */
int obc_available_update(void);

/**
 * @brief Polls for available PAY firmware upate
 */
int pay_available_update(void);

/**
 * @brief Executes OBC firmware update
 */
int obc_update(void);

/**
 * @brief Executes PAY firmware update
 */
int pay_update(void);

#endif
