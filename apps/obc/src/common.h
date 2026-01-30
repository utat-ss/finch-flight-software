/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file common.h
 * @brief Header for Definitions Common Across Modules 
 */

#ifndef COMMON_H
#define COMMON_H

/**
 * @brief System modules
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
 * @brief Modes of operation
 */
typedef enum mode_op {
	MODE_OP_IDLE,
	MODE_OP_IMAGING,
	MODE_OP_DOWNLINKING,
	MODE_OP_SAFETY,
	MODE_OP_FIRMWARE_UPDATE
} mode_op;

/**
 * @brief ADCS control modes
 */
typedef enum mode_adcs {
	MODE_ADCS_SAFE,
	MODE_ADCS_DETUMBLING,
	MODE_ADCS_SUN_POINTING,
	MODE_ADCS_FINE_POINTING,
	MODE_ADCS_LVLH,
	MODE_ADCS_TRACKING,
	MODE_ADCS_OFF
} mode_adcs;

/**
 * @brief PAY control mode
 */
typedef enum mode_pay {
	MODE_PAY_OFF,
	MODE_PAY_ON
} mode_pay;

/**
 * @brief Enter specified mode of operation
 */
void enter_mode_op(mode_op mode);

/**
 * @brief Enter specified ADCS control mode
 */
void cmd_adcs_mode(mode_adcs mode, char* orbit_info, float current_time, char* tle);

/**
 * @brief Enter specified PAY control mode
 */
void cmd_pay_mode(mode_pay mode);

#endif