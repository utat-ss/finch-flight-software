#include <finch/ccsds123b/predictor.h>
#include <finch/ccsds123b/encoder.h>
#include <finch/ccsds123b/image.h>
#include <finch/ccsds123b/util.h>

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

LOG_MODULE_REGISTER(bench);

timing_t start_time;

void measure_start() {
	timing_init();
	timing_start();
	start_time = timing_counter_get();
}

uint64_t timing_measure_ms() {
	timing_t end_time = timing_counter_get();
	uint64_t total_cycles = timing_cycles_get(&start_time, &end_time);
	uint64_t total_ns = timing_cycles_to_ns(total_cycles);
	LOG_INF("total_cycles: %llu", total_cycles);
	return total_ns / 1000000;
}

int main(void)
{
	measure_start();

	img_init();
	const vec3 *N = img_get_N();

	LOG_INF("Image initialized -- %llu ms", timing_measure_ms());

	int prediction[N->z][N->y][N->x];

	LOG_INF("Calling predict_image");

	predict_image(N, prediction);

	LOG_INF("Finished predict_image -- %llu ms since start", timing_measure_ms());
	log_flush();

	timing_stop();

	/*char encoding[N->z][N->y][N->x][32];

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
	}*/
}
