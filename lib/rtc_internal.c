/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/sys/util.h>
#include <time.h>

int set_date_time(const struct device *rtc, const struct tm *t)
{
    if (t == NULL) {
        printk("NULL time pointer passed in\n");
        return -EINVAL;
    }

    struct rtc_time tm = {
        .tm_year = t->tm_year,
        .tm_mon  = t->tm_mon,
        .tm_mday = t->tm_mday,
        .tm_hour = t->tm_hour,
        .tm_min  = t->tm_min,
        .tm_sec  = t->tm_sec,
    };

    int ret = rtc_set_time(rtc, &tm);
    if (ret < 0) {
        printk("Cannot write date time: %d\n", ret);
    }

    return ret;
}

int get_date_time(const struct device *rtc, struct tm *t)
{
    if (t == NULL) {
        printk("NULL time pointer passed in\n");
        return -EINVAL;
    }

    struct rtc_time tm;
    int ret = rtc_get_time(rtc, &tm);
    if (ret < 0) {
        printk("Cannot read date time: %d\n", ret);
        return ret;
    }

    // Copy fields to the output parameter
    t->tm_year = tm.tm_year;
    t->tm_mon  = tm.tm_mon;
    t->tm_mday = tm.tm_mday;
    t->tm_hour = tm.tm_hour;
    t->tm_min  = tm.tm_min;
    t->tm_sec  = tm.tm_sec;

    return ret;
}
