/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/ccsds123b/predictor.h>
#include <finch/ccsds123b/encoder.h>
#include <finch/ccsds123b/image.h>
#include <finch/ccsds123b/util.h>
#include <finch/ccsds123b/constants.h>

#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/ztest.h>

Predictions prediction;
EncoderOut encoding;

LOG_MODULE_REGISTER(bench);

ZTEST_SUITE(benchmark_test, NULL, NULL, NULL, NULL, NULL);

ZTEST(benchmark_test, test_benchmark)
{
	LOG_INF("Starting the image compression benchmark.");
	LOG_INF("N_Z = %d, N_Y = %d, N_X = %d", N_Z, N_Y, N_X);
	log_flush();

	img_init();
	const vec3 *N = img_get_N();

	LOG_INF("Image initialized");
	LOG_INF("Calling predict_image");
	log_flush();

	predict_image(N, prediction);

	LOG_INF("Finished predict_image");
	LOG_INF("Calling encode_prediction");
	log_flush();

	encode_prediction(N, prediction, encoding);

	LOG_INF("Finished encode_prediction");
	LOG_INF("Image compression benchmark has finished!");
	log_flush();
}
