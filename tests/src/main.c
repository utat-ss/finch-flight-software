/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <time.h>

#include "rtc_internal.h"  // Includes set_date_time and get_date_time

const struct device *const rtc = DEVICE_DT_GET(DT_NODELABEL(rtc));

void print_time(const struct tm *t)
{
    printk("RTC date and time: %04d-%02d-%02d %02d:%02d:%02d\n",
           t->tm_year + 1900,
           t->tm_mon + 1,
           t->tm_mday,
           t->tm_hour,
           t->tm_min,
           t->tm_sec);
}

int main(void)
{
    if (!device_is_ready(rtc)) {
        printk("Device is not ready\n");
        return 0;
    }

    // Prepare the time to set
    struct tm new_time = {
        .tm_year = 2025 - 1900,
        .tm_mon  = 7 - 1,
        .tm_mday = 5,
        .tm_hour = 12,
        .tm_min  = 0,
        .tm_sec  = 0,
    };

    // Set the RTC to this time
    int ret = set_date_time(rtc, &new_time);
    if (ret < 0) {
        printk("Failed to set time: %d\n", ret);
    }

    // Continuously read and print the time every second
    struct tm current_time;
    while (1) {
        ret = get_date_time(rtc, &current_time);
        if (ret == 0) {
            print_time(&current_time);
        }
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
