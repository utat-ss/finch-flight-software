/*
 * Copyright (c) 2024, Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/util.h>

const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(mcp7940n));
const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(usart1));

void print(const char* str)
{
	for (int i = 0; i < strlen(str); ++i) {
		uart_poll_out(uart, str[i]);
	}
}

static int set_date_time(const struct device *rtc)
{
	int ret = 0;
	struct rtc_time tm = {
		.tm_year = 2024 - 1900,
		.tm_mon = 11 - 1,
		.tm_mday = 17,
		.tm_hour = 4,
		.tm_min = 19,
		.tm_sec = 0,
	};

	ret = rtc_set_time(rtc, &tm);
	if (ret < 0) {
		return ret;
	}
	return ret;
}

int main(void)
{
	print("hello, zephyr!\r\n");
		
	/* Check if the RTC is ready */
	if (!device_is_ready(rtc)) {
		print("rip\r\n");
		return 0;
	}

	set_date_time(rtc);

	/* Continuously read the current date and time from the RTC */
	while (1) {
		struct rtc_time tm;
		rtc_get_time(rtc, &tm);

		char buf[64];
		snprintf(buf, sizeof(buf), "time: %d\r\n", tm.tm_sec);
		for (int i = 0; i < strlen(buf); ++i) {
			uart_poll_out(uart, buf[i]);
		}

		k_msleep(1000);
	};
	return 0;
}