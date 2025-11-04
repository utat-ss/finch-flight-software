/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FINCH_ADCS_H
#define FINCH_ADCS_H

#include <stdint.h>

#define ADCS_ID_SIZE 12

typedef enum {
	ADCS_RET_OK,
	ADCS_RET_ERR,
} adcs_ret;

adcs_ret adcs_init();

/**
 * @brief Gets the ID of the ADCS module.
 *
 * Note the ID of the dev board is different than the actual module.
 *
 * @param id	ID of the ADCS module.
 *
 * @retval ADCS_RET_OK   On success.
 * @retval ADCS_RET_ERR  On failure.
 */
adcs_ret adcs_get_id(uint8_t *id);

#endif /* FINCH_ADCS_H */
