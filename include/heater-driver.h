/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HEATER_DRIVER_H
#define HEATER_DRIVER_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdbool.h>

/**
 * Reads the temperature (ADC value) from PA0 (ADC1 channel 0).
 * Returns: 12-bit ADC value on success (0–4095), -1 on failure.
 */
int read_temperature(const struct device *adc_dev);

/**
 * Turns the heater on or off via GPIO (PA1 in this example).
 * `state` = 1 to turn ON, 0 to turn OFF.
 */
void control_heater(const struct device *gpio_dev, bool state);

// Determine when to turn on the temperature based on the threshold
bool state (short int temperature);


#endif