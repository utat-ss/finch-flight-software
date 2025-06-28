/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/devicetree.h>

#define TEMP_THRESHOLD 2000  // Adjust based on testing (e.g. 12-bit ADC ~ 0-4095)

void main(void)
{
    const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc1));
    const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpioa));

    printk("Starting heater controller...\n");

    while (1) {
        int temp = read_temperature(adc_dev);

        if (temp < 0) {
            printk("ADC read failed\n");
        } else {
        	printk("Temperature (ADC): %d\n", temp);

        	if (temp < TEMP_THRESHOLD) {
                printk("Temperature low, turning heater ON\n");
                control_heater(gpio_dev, 1);
            } else {
                printk("Temperature sufficient, turning heater OFF\n");
                control_heater(gpio_dev, 0);
            }
        }

        k_msleep(1000);  // Sample once per second
    }
}

