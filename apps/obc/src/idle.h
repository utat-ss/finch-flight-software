/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file idle.h
 * @brief Header for Idle Command Sequence Logic
 */

#ifndef idle_h
#define idle_h

#include <stdbool.h>

/**
 * @brief system modules involved in idle command sequence
 */
typedef enum system_module {
	MODULE_OPERATOR,
	MODULE_MCC_GS,
	MODULE_RF,
	MODULE_OBC,
	MODULE_ADCS,
	MODULE_PAY
} system_module;

/**
 * @brief executes the idle command sequence
 */
int idle_command_sequence(void);

#endif
