#include "predictor.h"
#include "image.h"
#include "util.h"

#include <stdio.h>
#include <zephyr/kernel.h>

int main()
{
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
}
