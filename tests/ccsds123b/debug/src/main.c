/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "reconstructor.h"

#include <finch/ccsds123b/encoder.h>
#include <finch/ccsds123b/image.h>
#include <finch/ccsds123b/predictor.h>
#include <finch/ccsds123b/util.h>

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

ZTEST_SUITE(debug_test, NULL, NULL, NULL, NULL, NULL);

ZTEST(debug_test, test_stdout) {
	img_init();
	const vec3 *N = img_get_N();

	printk("-----------------------------------\nImage\n\n");
	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x)
				printk("%d ", img_get_pxl(z, y, x));
			printk("\n");
		}
		printk("\n");
	}

	int prediction[N->z][N->y][N->x];

	predict_image(N, prediction);

	printk("-----------------------------------\nPrediction\n\n");
	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x)
				printk("%d ", prediction[z][y][x]);
			printk("\n");
		}
		printk("\n");
	}

	char encoding[N->z][N->y][N->x][32];

	encode_prediction(N, prediction, encoding);

	printk("-----------------------------------\nEncoding\n\n");
	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x)
				printk("%s ", encoding[z][y][x]);
			printk("\n");
		}
		printk("\n");
	}

	int prediction_new[N->z][N->y][N->x];

	decode_encoding(N, encoding, prediction_new);

	printk("-----------------------------------\nDecoding\n\n");
	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x)
				printk("%d ", prediction_new[z][y][x]);
			printk("\n");
		}
		printk("\n");
	}

	int image_new[N->z][N->y][N->x];

	reconstruct_prediction(N, prediction_new, image_new);

	printk("-----------------------------------\nReconstruction\n\n");
	for (int z = 0; z < N->z; ++z) {
		for (int y = 0; y < N->y; ++y) {
			for (int x = 0; x < N->x; ++x)
				printk("%d ", image_new[z][y][x]);
			printk("\n");
		}
		printk("\n");
	}
}
