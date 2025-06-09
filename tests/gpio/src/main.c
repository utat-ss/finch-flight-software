/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(gpio);

#define TOGGLE_GPIO_DELAY_MS 2500

#define TOGGLE_GPIO(id)                                                                            \
	{                                                                                          \
		const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET(id, gpios);                      \
		if (device_is_ready(gpio.port)) {                                                  \
			gpio_pin_configure_dt(&gpio, GPIO_OUTPUT_ACTIVE);                          \
			gpio_pin_set_dt(&gpio, 0);                                                 \
			k_msleep(TOGGLE_GPIO_DELAY_MS);                                            \
			gpio_pin_set_dt(&gpio, 1);                                                 \
			k_msleep(TOGGLE_GPIO_DELAY_MS);                                            \
		}                                                                                  \
	}

int main(void)
{
	while (1) {
		DT_FOREACH_CHILD(DT_PATH(gpio_pins), TOGGLE_GPIO);
	}
	return 0;
}
