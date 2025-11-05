/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pay);

int main(void)
{
    const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(usart3));
    if (!uart_dev) {
        printk("UART device not found!\n");
        return 0;
    }

    while (1) {
        const char msg[] = "Hello World\n";
        for (int i = 0; i < sizeof(msg) - 1; i++) {
            uart_poll_out(uart_dev, msg[i]);
        }
        printk("Sent: %s", msg);
        k_msleep(2000);
    }

    return 0;
}
