/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file downlinking.h
 * @brief Header for Downlinking Command Sequence Implementation
 */

#ifndef DOWNLINKING_H
#define DOWNLINKING_H

#include "common.h"

/**
 * @brief Downlink types
 */
typedef enum downlink_type {
  DOWNLINK_TELEMETRY,
	DOWNLINK_IMAGE
} downlink_type;

/**
 * @brief Main loop for Downlinking command sequence
 */
int downlinking_command_sequence(void);

/**
 * @brief Prepare RF for downlinking
 */
downlink_type cmd_rf_prepare_downlink(void);

/**
 * @brief Get telemetry data for downlinking
 */
char* cmd_get_telemetry_data(void);

/**
 * @brief Get image data for downlinking
 */
char* cmd_get_image_data(void);

/**
 * @brief Downlink data to RF
 */
int cmd_rf_downlink_data(char* downlink_data);

#endif