/*
 * Copyright (c) 2024, Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <zephyr/kernel.h>
 #include <zephyr/device.h>
 #include <zephyr/drivers/rtc.h>
 #include <zephyr/sys/util.h>
 
 const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(rtc));


//this is for setting the time
static int set_date_time(const struct device *rtc)
{
    int ret = 0;
    struct rtc_time tm = {
        .tm_year = 2025 - 1900,
        .tm_mon = 5 - 1,
        .tm_mday = 17,
        .tm_hour = 4,
        .tm_min = 11,
        .tm_sec = 0,
    };

    ret = rtc_set_time(rtc, &tm);
    if (ret < 0) { //error as the return value is not right
        printk("Error in writing the date time: %d\n", ret);
        return ret;
    }
    return ret;
}

//this code is for getting the time
static int get_date_time(const struct device *rtc)
{
    int ret = 0;
    struct rtc_time tm;

    //retrieve the current time from the RTC
    ret = rtc_get_time(rtc, &tm); //ret has the status code
    if (ret < 0) { //error as the return value was not right
        printk("Error in reading the date time: %d\n", ret);
        return ret;
    }

    printk("RTC date and time: %04d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return ret;
}

int main(void)
{
    
    /* Check if the RTC is ready */
    if (!device_is_ready(rtc)) {
        printk("The device is not ready\n");
        return 0;
    }

    set_date_time(rtc); //sets the time on the RTC

    /* Continuously read the current date and time from the RTC */
    // Loops forever, calling get_date_time() every 1000 ms (1 second):
    while (get_date_time(rtc) == 0) {
        k_sleep(K_MSEC(1000));
    };
    
    return 0;
}


