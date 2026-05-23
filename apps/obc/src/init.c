/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file init.c
 * @brief Init Command Sequence Implementation
 */

#include "common.h"
#include "imaging.h"
#include <stdbool.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(initializing_sequence);

int init() {
    mode_op operation = MODE_OP_IDLE;

    while (1) {
        switch (operation) {
            case MODE_OP_IDLE:
                LOG_INF("Entering Idle mode");

                operation = idle_command_sequence();
                
                break;

            case MODE_OP_IMAGING:
                LOG_INF("Entering Imaging mode");

                operation = imaging_command_sequence();

                break;

            case MODE_OP_DOWNLINKING:
                LOG_INF("Entering Downlinking mode");

                operation = downlinking_command_sequence();
                
                break;

            case MODE_OP_SAFETY:
                LOG_INF("Entering Safety mode");

                operation = safety_command_sequence();

                break;

            case MODE_OP_FIRMWARE_UPDATE:
                LOG_INF("Entering Firmware Update mode");

                operation = firmware_update_command_sequence();

                break;

            default:
                LOG_WRN("Unknown mode of operation");

                operation = MODE_OP_SAFETY;

                break;
        }
    }
}

