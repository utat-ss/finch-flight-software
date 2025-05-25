#include "predictor.h"
#include "encoder.h"
#include "reconstructor.h"

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <time.h>

void print_i(int img[IMG_Z][IMG_Y][IMG_X])
{
	for (int i = 0; i < IMG_Z; ++i)
	{
		for (int j = 0; j < IMG_Y; ++j)
		{
			for (int k = 0; k < IMG_X; ++k)
				printk("%d ", img[i][j][k]);
			printk("\n");
		}
		printk("\n");
	}
	printk("\n");
}

void print_s(char img[IMG_Z][IMG_Y][IMG_X][32])
{
	for (int i = 0; i < IMG_Z; ++i)
	{
		for (int j = 0; j < IMG_Y; ++j)
		{
			for (int k = 0; k < IMG_X; ++k)
				printk("%s ", img[i][j][k]);
			printk("\n");
		}
		printk("\n");
	}
	printk("\n");
}

float spectral_signature(int band, int type)
{
    if (type == 0) // "Field" - gradual increase
        return 50.0 + (band * 5.0); // Linear increase
    else// "Road" - flatter with a peak
        return 100.0 + (band < 16 ? band * 2.0 : 32 - band);
}

int main()
{
	srand(time(0));
	int32_t img[IMG_Z][IMG_Y][IMG_X];
	
	for (int i = 0, c = 0; i < IMG_Z; ++i)
		for (int j = 0; j < IMG_Y; ++j)
			for (int k = 0; k < IMG_X; ++k)
				img[i][j][k] = ++c;

	// for (int y = 0; y < IMG_Y; y++) {
	// 	for (int x = 0; x < IMG_X; x++) {
	// 		int type = (y < IMG_Y / 2) ? 0 : 1; // Top half "field", bottom half "road"
	// 		for (int b = 0; b < IMG_Z; b++) {
	// 			// Base value from spectral signature
	// 			img[b][y][x] = spectral_signature(b, type);
	// 			// Add random noise (±5 intensity units)
	// 			img[b][y][x] += (rand() % 11 - 5);
	// 			// Clamp values to a realistic range (e.g., 0-255)
	// 			if (img[b][y][x] < 0) img[b][y][x] = 0;
	// 			if (img[b][y][x] > 255) img[b][y][x] = 255;
	// 		}
	// 	}
	// }

	printk("Image\n\n");
	print_i(img);
	
	printk("Prediction\n\n");
	int32_t prediction[IMG_Z][IMG_Y][IMG_X];
	predict_image(img, prediction);
	print_i(prediction);

	printk("Encoding\n\n");
	char encoding[IMG_Z][IMG_Y][IMG_X][32];
	encode_prediction(prediction, encoding);
	print_s(encoding);

	printk("Decoding\n\n");
	int decoding[IMG_Z][IMG_Y][IMG_X];
	decode_encoding(encoding, decoding);
	print_i(decoding);

	return 0;
}