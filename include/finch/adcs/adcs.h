/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief The library for controlling the Attitude Determination and Control System (ADCS)
 *
 * The ADCS module controls the attitude of the satellite. The OBC communicates to the ADCS module
 * via UART. A typical workflow is asking the ADCS module the current attitude, checking if it's
 * nominal, asking it to correct the attitude if not nominal, and repeat.
 *
 * We develop and test our code on the ADCS dev board first. This dev board mimics the commands the
 * actual ADCS module is expected to receive and send. When we are confident in our approach, we
 * will then test on the actual ADCS module. We have the same codebase and testsuite for both the
 * dev board and the actual module.
 */

#ifndef FINCH_ADCS_H
#define FINCH_ADCS_H

#include <stdlib.h>

/* Datasheet specifics the ID to be 12 bytes, so we store as 16 bytes. */
typedef uint16_t adcs_id_t;

/* The get_id function can either fail or not, so return binary value. */
typedef enum {
	ADCS_GET_ID_RET_OK,
	ADCS_GET_ID_RET_ERR,
} adcs_get_id_ret;

/**
 * @brief Gets the ID of the ADCS module.
 *
 * Note the ID of the dev board is different than the actual module.
 *
 * @param id    ID of the ADCS module.
 *
 * @retval ADCS_GET_ID_RET_OK   On success.
 * @retval ADCS_GET_ID_RET_ERR  On failure.
 */
adcs_get_id_ret adcs_get_id(adcs_id_t *id);

#endif /* FINCH_ADCS_H */
