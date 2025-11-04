/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_CSP_CSP_H_
#define FINCH_CSP_CSP_H_

#include <stdint.h>

/* CSP node addresses for FINCH subsystems. */
#define FINCH_CSP_ADDR_OBC 1U
#define FINCH_CSP_ADDR_PAY 2U

/* OBC application CSP port (commands and responses). */
#define FINCH_CSP_OBC_PORT 10U

/* OBC command identifiers (encoded as a 16-bit big-endian value in the
 * first two bytes of the CSP packet payload).
 */
#define FINCH_CSP_CMD_ADCS_GET_ID 0x0000U

/**
 * @brief Initialise the FINCH CSP integration layer.
 *
 * Initialises libcsp, binds the standard service handlers (ping, uptime,
 * etc.), brings up the configured transports (loopback always, CAN when
 * @kconfig{CONFIG_FINCH_CSP_HAVE_CAN} is enabled), and starts the CSP
 * router thread. Safe to call more than once.
 *
 * @retval 0 on success.
 * @retval -EIO if CSP services or transports could not be brought up.
 * @retval -ENODEV if the CAN device is not ready.
 */
int finch_csp_init(void);

#endif /* FINCH_CSP_CSP_H_ */
