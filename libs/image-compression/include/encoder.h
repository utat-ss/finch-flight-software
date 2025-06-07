#pragma once

#include "global.h"

/*
 * [5.4.2.3] Band Sequential Order.
 */
void encode_prediction(
	int prediction[IMG_Z][IMG_Y][IMG_X],
	char out[IMG_Z][IMG_Y][IMG_X][32]
);

/*
 * [5.4.3.2.2.1] Length limited Golomb-power-of-2 (GPO2)
 */
void encode_gpo2(
	int j, int k,
	char out[32]
);
