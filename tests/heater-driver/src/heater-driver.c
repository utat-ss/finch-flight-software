/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <heater-driver.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>

#define ADC_CHANNEL     0
#define ADC_INPUT       0
#define ADC_RESOLUTION  12
#define HEATER_PIN      1   // PA1

int read_temperature(const struct device *adc_dev)
{
    if (!device_is_ready(adc_dev)) {
        return -1;
    }

    int16_t sample_buffer;
    struct adc_channel_cfg channel_cfg = {
        .gain             = ADC_GAIN_1,
        .reference        = ADC_REF_INTERNAL,
        .acquisition_time = ADC_ACQ_TIME_DEFAULT,
        .channel_id       = ADC_CHANNEL,
        .differential     = 0,
        .input_positive   = ADC_INPUT,
    };

    struct adc_sequence sequence = {
        .channels    = BIT(ADC_CHANNEL),
        .buffer      = &sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution  = ADC_RESOLUTION,
    };

    if (adc_channel_setup(adc_dev, &channel_cfg) < 0) {
        return -1;
    }

    if (adc_read(adc_dev, &sequence) < 0) {
        return -1;
    }

    return sample_buffer;
}

void control_heater(const struct device *gpio_dev, bool state)
{
    if (!device_is_ready(gpio_dev)) {
        return;
    }

    gpio_pin_configure(gpio_dev, HEATER_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set(gpio_dev, HEATER_PIN, state);
}