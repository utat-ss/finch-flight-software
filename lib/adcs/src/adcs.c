/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>

adcs_get_id_ret adcs_get_id(adcs_id_t *id)
{
	*id = 0x54;
	return ADCS_GET_ID_RET_OK;
}
