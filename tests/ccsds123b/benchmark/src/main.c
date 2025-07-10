#include <finch/ccsds123b/predictor.h>
#include <finch/ccsds123b/encoder.h>
#include <finch/ccsds123b/image.h>
#include <finch/ccsds123b/util.h>
#include <finch/ccsds123b/constants.h>

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>


Predictions prediction;
EncoderOut encoding;

LOG_MODULE_REGISTER(bench);

timing_t start_time;

void measure_start(void)
{
	timing_init();
	timing_start();
	start_time = timing_counter_get();
}

uint64_t timing_measure_ms(void)
{
	timing_t end_time = timing_counter_get();
	uint64_t total_cycles = timing_cycles_get(&start_time, &end_time);
	uint64_t total_ns = timing_cycles_to_ns(total_cycles);

	return total_ns / 1000000;
}

int main(void)
{
	LOG_INF("Starting the image compression benchmark.");
	LOG_INF("N_Z = %d, N_Y = %d, N_X = %d", N_Z, N_Y, N_X);
	log_flush();

	measure_start();

	img_init();
	const vec3 *N = img_get_N();

	LOG_INF("Image initialized -- %llu ms", timing_measure_ms());

	LOG_INF("Calling predict_image");

	predict_image(N, prediction);

	LOG_INF("predict_image took %llu ms to compute", timing_measure_ms());

	timing_stop();
	measure_start();

	encode_prediction(N, prediction, encoding);

	LOG_INF("encode_prediction took %llu ms to compute", timing_measure_ms());

	timing_stop();
	log_flush();
}
