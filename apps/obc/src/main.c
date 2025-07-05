/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/sys/util.h>


const struct device *const rtc = DEVICE_DT_GET(DT_NODELABEL(rtc));

static int set_date_time(const struct device *rtc)
{
    int ret = 0;

    // Initialize a 'rtc_time' struct with the desired date and time
    // Note: tm_year is years since 1900, tm_mon is 0-based (0 = January)
    struct rtc_time tm = {
        .tm_year = 2025 - 1900,  // Year 2025
        .tm_mon = 6 - 1,        // November (10 in 0-based index)
        .tm_mday = 7,           // Day of the month
        .tm_hour = 15,            // Hour (24-hour format)
        .tm_min = 50,            // Minutes
        .tm_sec = 0,             // Seconds
    };

    // Set the RTC with the specified time
    ret = rtc_set_time(rtc, &tm);
    if (ret < 0) {
        // If setting the time failed, print an error message with the error code
        printk("Cannot write date time: %d\n", ret);
        return ret;
    }

    // Return 0 if successful
    return ret;
}



// Function to get and print the RTC date and time
static int get_date_time(const struct device *rtc)
{
    int ret = 0;
    struct rtc_time tm;  // Structure to hold the retrieved time

    // Get the current time from the RTC
    ret = rtc_get_time(rtc, &tm);
    if (ret < 0) {
        // If reading the time failed, print an error message with the error code
        printk("Cannot read date time: %d\n", ret);
        return ret;
    }

    // Print the time in human-readable format (convert year and month back)
    printk("RTC date and time: %04d-%02d-%02d %02d:%02d:%02d\n",
           tm.tm_year + 1900,   // Convert back to full year
           tm.tm_mon + 1,       // Convert back to 1-based month
           tm.tm_mday,
           tm.tm_hour,
           tm.tm_min,
           tm.tm_sec);

    return ret;
}


int main(void)
{
    // Check if the RTC device is ready for use
    if (!device_is_ready(rtc)) {
        printk("Device is not ready\n");
        return 0;  // Exit if RTC is not ready
    }

    // Set the date and time on the RTC
    set_date_time(rtc);

    // Infinite loop: continuously read and display the RTC date/time every 800ms
   while (get_date_time(rtc) ==0){
    k_sleep(K_MSEC(1000));
   }
	return 0;

}

