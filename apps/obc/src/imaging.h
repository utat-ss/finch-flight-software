/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file imaging.h
 * @brief Header for Imaging Command Sequence Implementation
 */

#ifndef IMAGING_H
#define IMAGING_H

#include "common.h"

/**
 * @brief Main loop for Imaging command sequence
 */
int imaging_command_sequence(void);

/**
 * @brief Execute camera cooling
 */
int cmd_pay_camera_cool(void);

/**
 * @brief Execute image capture
 */
int cmd_pay_camera_image(void);

#endif