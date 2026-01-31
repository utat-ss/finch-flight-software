/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file safety.c
 * @brief Safety Command Sequence Implementation
 */

#include "common.h"
#include "safety.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);
LOG_MODULE_REGISTER(safety, LOG_LEVEL_INF);

/**
 * @brief Main loop for Safety command sequence
 */
int safety_command_sequence(void)
{
  cmd_pay_off();
  cmd_adcs_mode(MODE_ADCS_OFF, 0, 0.0, 0); // Placeholder values for orbit_info, current_time and tle

  uint8_t* error_info;
  safety_command cmd;

  while (1)
  {
    // Receive ping

    error_info = cmd_rf_get_error_info();
    cmd_rf_send_error_info(error_info);

    cmd = cmd_rf_get_error_handler();
    cmd_rf_execute_error_handler(cmd);

    // Check if operator commands to exit Safety mode
    if (cmd_rf_check_exit_safety_mode())
    {
      cmd_pay_mode(MODE_PAY_ON);
      cmd_adcs_mode(MODE_ADCS_SUN_POINTING, 0, 0.0, 0); // Placeholder values for orbit_info, current_time and tle

      // Send to RF that exiting Safety mode

      enter_mode_op(MODE_OP_IDLE);
      return 0;
    }
  }
}

/**
 * @brief Get error info from RF
 */
uint8_t* cmd_rf_get_error_info(void)
{
  // Get error info from RF
  return NULL;
}

/**
 * @brief Get error handler from RF
 */
safety_command cmd_rf_get_error_handler(void)
{
  // Get error handler from RF
  return NULL;
}

/**
 * @brief Send error info to RF
 */
int cmd_rf_send_error_info(uint8_t* error_info)
{
  // Send error info to RF
  return 0;
}

/**
 * @brief Send error handler command to RF
 */
int cmd_rf_execute_error_handler(safety_command cmd)
{
  // Send error handler command to RF
  return 0;
}

/**
 * @brief Check if operator commands to exit Safety mode
 */
int cmd_rf_check_exit_safety_mode(void)
{
  // Check if operator commands to exit Safety mode
  return 0;
}