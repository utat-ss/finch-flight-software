/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTC_INTERNAL_H
#define RTC_INTERNAL_H

#include <zephyr/device.h>
#include <time.h>  // for struct tm

int set_date_time(const struct device *rtc, const struct tm *t);
int get_date_time(const struct device *rtc, struct tm *t);

#endif  // RTC_INTERNAL_H
