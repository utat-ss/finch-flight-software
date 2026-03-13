/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * ADCS header and source files provide a library to communicate with the ADCS dev board via UART.
 */

#ifndef FINCH_INCLUDE_ADCS_H
#define FINCH_INCLUDE_ADCS_H

#include <stdint.h>

#include <zephyr/device.h>

#define FINCH_ADCS_SNID_LEN 12

typedef enum {
        FINCH_ADCS_REGISTER_SNID
} finch_adcs_register;

typedef enum {
	FINCH_ADCS_RET_OK,
        FINCH_ADCS_RET_NONE_READ
} finch_adcs_ret;

/**
 * @brief Check if UART is ready.
 */
finch_adcs_ret finch_adcs_init(void);

/**
 * @brief Read a register on the ADCS dev board.
 * @see 7.1 Serial Communication Protocol.
 *
 * Every read register command contains a 4 byte header followed by a checksum.
 */
finch_adcs_ret finch_adcs_register_read(
        finch_adcs_register reg,
        uint8_t *rx,
        uint8_t rx_len
);

#endif /* FINCH_INCLUDE_ADCS_H */
